/******************************************************************
 * Filename   :conrol.c                                           *
 * Description:control the door and store password                *
 *            and manipulate password verification                *
 * Created on: Oct 02, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

#include "external_eeprom.h"
#include "timer.h"
#include "uart.h"
#include <util/delay.h>


#define CLOCKWISE 0
#define ANTICLOCKWISE 1


/*******************UART Control Messages*************************/
#ifndef ERROR
#define ERROR 0
#define SUCCESS 1
#endif

#define NO_PASSWORD      0X01  /* message send by Control ECU to HMI ECU to inform that there is no password in EEPROM (new device) */
#define PASSWORD_EXIST   0X02  /* message send by Control ECU to HMI ECU to inform that there is a password in EEPROM*/
#define NOT_MATCHED      0X03  /* message send by Control ECU to HMI ECU to inform that the password not matched */
#define MATCHED          0X04  /* message send by Control ECU to HMI ECU to inform that the passwords is matched */
#define UNLOCK_ORDER     0x05  /* message send by HMI ECU to Control ECU TO ask to start process of unlock door */
#define CHANGE_PASSWORD  0X06  /* message send by HMI ECU to Control ECU TO ask to change password */

/***************Global variables********************************/
uint32 g_password =0;         /* Variable to hold password */
uint32 g_tempPassword=0;      /* variable to hold the temp password while change password process */
uint8 g_WrongAttemp=0;        /* number of wrong password entered */
volatile uint8 g_Tick=0;

/************* functions Prototype ******************************/
void GetPasswordEEPROM(void);
void StorePasswordEEPROM(void);
void ChangePassword(void);
void ReceivePassword(uint32 *password);
uint8 VerifyPassword(void);
void UnlockDoor(void);
void TimerCallBack(void);
void MotorInit(void);
void MotorON(uint8 state);
void MotorOFF(void);
void BuzzerInit(void);
void BuzzerON(void);
void BuzzerOFF(void);
void PWM_Timer0_Init(uint8 set_duty_cycle);
void PWM_Timer0_Deinit(void);
void ActivateAlarm(void);


void main(void)
{
	uint8 status,permission;
	UART_init();
	EEPROM_init();
	MotorInit();
	BuzzerInit();
	/* Enable Global Interrupt */
	SET_BIT(SREG,7);
	_delay_ms(100);      /* Wait for EEPROM and HMI to get ready to receive */
	GetPasswordEEPROM(); /* get password from External EEPROM and store it in g_password variable */
	if(g_password==0)    /* device is new and there is no password exist in External EEPROM*/
	{
		UART_sendByte(NO_PASSWORD);
		ChangePassword();  /* Call Function to get new password from User (HMI ECU)*/
	}
	else
	{
		/* make sure to send byte because the HMI waits for response */
		UART_sendByte(PASSWORD_EXIST);
	}
	while(1)
	{
		status=UART_receiveByte();
		if(status ==UNLOCK_ORDER)
		{
			permission=VerifyPassword();
			if(permission==SUCCESS)
			{
				UnlockDoor();
			}
			else if (permission==ERROR)
			{
				ActivateAlarm();
			}

		}
		else if(status == CHANGE_PASSWORD)
		{
			permission=VerifyPassword();
			if(permission==SUCCESS)
			{
				ChangePassword();
			}
			else if (permission==ERROR)
			{
				ActivateAlarm();
			}

		}

	}
}

void GetPasswordEEPROM(void)
{
	/*this function is responsible of getting stored password from External EEPROM*/

	uint8 *ptr=&g_password;    /* Pointer to char  to store password byte by byte */
	uint8 i=0;
	for(i=0;i<4;i++)                    /* get password from EEPROM 4 BYTE */
	{
		EEPROM_readByte((0X0010+i),ptr);
		ptr++;
		_delay_ms(10);         /* delay for EEPROM to be ready for next byte */
	}
}

void StorePasswordEEPROM(void)
{
	/* function to store password to External EEPROM*/
	uint8 *ptr=&g_tempPassword;    /* Pointer to char  to store password byte by byte */
	uint8 i=0;
	for(i=0;i<4;i++)                    /* send password to EEPROM 4 BYTE */
	{
		EEPROM_writeByte((0X0010+i),*ptr);
		ptr++;
		_delay_ms(10);                /* delay for EEPROM to be ready for next byte */
	}
}

void ReceivePassword(uint32 *password)
{
	/* function for receiving password from HMI ECU
	 * function use pointer to char (1byte) to
	 * store password directly to variable
	 */
	/*pointer ptr point to the first byte of required password storage variable */
	uint8 *ptr=password;
	uint8 i=0;
	for(i=0;i<4;i++)
	{
		*ptr=UART_receiveByte();
		ptr++;  /* move pointer to next byte */
	}

}

void ChangePassword(void)
{
	/*Receive first password and store it in g_password */
	ReceivePassword(&g_password);
	/* Receive second password and store it in g_tempPassord */
	ReceivePassword(&g_tempPassword);
	/* check if they matched */
	while(g_password != g_tempPassword)
	{
		/* Not matched entry from user
		 * Inform HMI and get new entry from user
		 */
		UART_sendByte(NOT_MATCHED);
		ReceivePassword(&g_password);
		ReceivePassword(&g_tempPassword);
	}
	UART_sendByte(MATCHED);  /* password is matched */
	StorePasswordEEPROM();   /* store new password in ROM */
	g_tempPassword=0;        /* clear temp password */
}

uint8 VerifyPassword(void)
{
	/* Function to authenticate password entered by user */
	/* receive password from HMI */
	ReceivePassword(&g_tempPassword);
	while((g_password != g_tempPassword) && g_WrongAttemp<2)
	{
		g_WrongAttemp++;                   /* increment number of wrong tries */
		UART_sendByte(NOT_MATCHED);        /* inform HMI that was a wrong password to star send a new one*/
		ReceivePassword(&g_tempPassword);  /* receive new password from HMI */
	}
	if(g_WrongAttemp<2)
	{
		/*Clear Wrong Attempts Counter */
		g_WrongAttemp=0;
		UART_sendByte(MATCHED);          /* right password */
		return SUCCESS;
	}
	else
	{
		UART_sendByte(NOT_MATCHED);
		return ERROR;
	}

}
void UnlockDoor(void)
{
	uint8 flag=0;
	/* second configuration for timer1 CTC Mode and compare register
	 * we need 15 second at 8Mhz CPU frequency and scaler 1024
	 * so we need 117187 cycle by timer
	 * timer 1 is 16 bit i.e 65536 cycle
	 * we need two compare at compare value 39062
	 */
	T_Config config={TIMER1,CTC,T_CLK_1024,39062};
	/* set call back function for timer 1 */
	TIMER_setCallBack(TIMER1,TimerCallBack);
	/* initialize timer  */
	TIMER_init(&config);
	/* PWM Initialization */
	PWM_Timer0_Init(128);
	/* Unlock the door */
	while(g_Tick<2)
	{
		/* flag for execute code only one time */
		if(flag==0)
		{
			MotorON(CLOCKWISE);
			flag=1;
		}
	}
	/* time for opened door */
	while(g_Tick<4)
	{
		/* flag for execute code only one time */
		if(flag==1)
		{
			MotorOFF();
			flag=0;
		}
	}
	/* display flashing locking on screen while motor is closing the lock */
	while(g_Tick<6)
	{
		/* flag for execute code only one time */
		if(flag==0)
		{
			MotorON(ANTICLOCKWISE);
			flag=1;
		}
	}
	/* Switch OFF Motor */
	MotorOFF();
	/* Disable timer1 */
	TIMER_deInit(TIMER1);
	/* disable PWM (save Power) */
	PWM_Timer0_Deinit();
	/*clear g_tick */
	g_Tick=0;
}

void TimerCallBack(void)
{
	g_Tick++; /* Increment flag every timer Interrupt */
}


void BuzzerInit(void)
{
	/* Configure PB2 as output */
	DDRB |=0X04;
}

void BuzzerON(void)
{
	/* SET PB2*/
	SET_BIT(PORTB,2);
}

void BuzzerOFF(void)
{
	/*Clear PB2 */
	CLEAR_BIT(PORTB,2);
}

void MotorInit(void)
{
	/* Configure PB0:1 as output */
	DDRB |=0X03;
}

void MotorON(uint8 state)
{
	switch(state)
	{
	   case CLOCKWISE:
		   PORTB =(PORTB & ~0X03) | 0X02;
		   break;
	   case ANTICLOCKWISE:
		   PORTB =(PORTB & ~0X03) | 0X01;
		   break;
	}
}

void MotorOFF(void)
{
	/* clear PB0:1 to switch off motor */
	PORTB &=~0X03;
}

void PWM_Timer0_Init(uint8 set_duty_cycle)
{
	TCNT0 = 0; //initial timer value
	OCR0  = set_duty_cycle;
	DDRB  = DDRB | (1<<PB3); //set OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}
void PWM_Timer0_Deinit(void)
{
	TCCR0=0;
	TCNT0 = 0;
	OCR0  =0;
}

void ActivateAlarm(void)
{
	/* configuration of timer */
	T_Config config={TIMER1,CTC,T_CLK_1024,58593};
	/* set call back function for timer 1 */
	TIMER_setCallBack(TIMER1,TimerCallBack);
	/* initialize timer  */
	TIMER_init(&config);
	/* turn on buzzer */
	BuzzerON();
	/* wait for timer */
	while(g_Tick !=8);
	/* turn Off Buzzer*/
	BuzzerOFF();
	/* Disable Timer */
	TIMER_deInit(TIMER1);
	/* Clear g_tick */
	g_Tick=0;
	/* clear g_WrongAttemp */
	g_WrongAttemp=0;
}
