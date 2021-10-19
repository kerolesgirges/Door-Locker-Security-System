/******************************************************************
 * Filename   :hmi.c                                              *
 * Description:interfacing with user                              *
 * Created on: Oct 01, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "uart.h"
#include <util/delay.h>


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


/***************Global variables*********************************/
uint32 g_tempPassword=0;
uint8 g_WrongAttemp=0;        /* number of wrong password entered */
volatile uint8 g_Tick=0;
uint8 g_AlarmCounter=0;       /* The number of times that the Alarm has been activated */

/************* functions Prototype ******************************/
void GetPassword(void);
void SendPassword(void);
void ChangePassword(void);
void DisplayHomeScreen(void);
uint8 VerifyPassword(void);
void UnlockDoor(void);
void TimerCallBack(void);
void ActivateAlarm(void);


void main(void)
{
	uint8 status,key,permission;
	UART_init();
	LCD_init();
	/* Enable Global Interrupt */
	SET_BIT(SREG,7);
	/* Receive existence of password in External EEPROM from Control ECU */
	status=UART_receiveByte();
	if(status==NO_PASSWORD)
	{
		ChangePassword();
	}
	while(1)
	{
		DisplayHomeScreen();
		key=KeyPad_getPressedKey();
		if(key=='+')
		{
			UART_sendByte(UNLOCK_ORDER);
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
		else if(key=='-')
		{
			UART_sendByte(CHANGE_PASSWORD);
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


void GetPassword(void)
{
	uint8 key;
	sint8 i=0;
	g_tempPassword=0;  /* clear tempPassword to store a new one */
	LCD_goToRowColumn(1,0);  /* got to the second row   */
	/* max number is 9 numbers
	 * exit the function if the user entered 9 numbers
	 * or pressed ON key 13
	 */
	for(i=0;i<9;i++)
	{
		key=KeyPad_getPressedKey();
		if((key>=0) && (key<=9))
		{
			/* store password as an integer number */
			g_tempPassword = (g_tempPassword * 10)+key;
			/* display '*' on screen  */
			LCD_displayCharacter('*');
		}
		else if(key==13)
		{
			/* force for loop to exit if user pressed 13*/
			i=9;
		}
		else
		{
			/*
			 * if user pressed any other key the function do not store it
			 * and i-- for keep the max entered numbers is 9
			 */
			i--;
		}
		_delay_ms(500);
	}
	LCD_displayStringRowColumn(1,0,"       ");
}

void SendPassword(void)
{
	/*
	 * function responsible for sending the password to Control ECU
	 */
	uint8 i=0;
	uint8 *ptr=&g_tempPassword;    /*a pointer to first byte of password */
	for(i=0;i<4;i++)
	{
		UART_sendByte(*ptr);
		/* move the pointer to next byte */
		ptr++;
	}
}

void ChangePassword(void)
{
	uint8 status=0;
	/* get first password from user and send it to Control ECU */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter 1st pass");
	GetPassword();
	SendPassword();
	/* get second password from user and send it to Control ECU */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter 2nd pass");
	GetPassword();
	SendPassword();
	/* check the result of matching from Control ECU */
	status = UART_receiveByte();
	while(status ==NOT_MATCHED)
	{
		/* if they unmatched display error on screen for 2 sec */
		LCD_displayStringRowColumn(0,0,"  NOT Matched");
		_delay_ms(2000);
		/* do it again till the password is matched */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Enter 1st pass");
		GetPassword();
		SendPassword();
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Enter 2nd pass");
		GetPassword();
		SendPassword();
		status = UART_receiveByte();
	}
	if(status == MATCHED)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"  Pass Changed");
		_delay_ms(2000);
	}
}


void DisplayHomeScreen(void)
{
	LCD_clearScreen();
	LCD_displayString("    Welcome");
	LCD_displayStringRowColumn(1,0,"(+):Unlock Door");
	LCD_displayStringRowColumn(2,0,"(-):Change Pass");
	/* The number of times that the Alarm has been activated */
	LCD_goToRowColumn(3,14);
	LCD_displayInteger(g_AlarmCounter);
}

uint8 VerifyPassword(void)
{
	uint8 status;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Pls Enter pass");
	GetPassword();                                        /*get Password From User */
	SendPassword();
	status=UART_receiveByte();                             /* receive status from control ECU */
	while((status== NOT_MATCHED) && g_WrongAttemp<2)
	{
		g_WrongAttemp++;                   /* increment number of wrong tries */
		/*
		 * if password was wrong get new one from user and send
		 * it to control ECU
		 * exit while loop if the password is correct or
		 * user try 3 wrong times
		 */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Wrong Password  ");
		_delay_ms(2000);
		LCD_displayStringRowColumn(0,0,"Pls Enter pass  ");
		GetPassword();                                       /*get Password From User */
		SendPassword();
		status=UART_receiveByte();
	}
	if(g_WrongAttemp<2)
	{
		/*Clear Wrong Attempts Counter */
		g_WrongAttemp=0;
		/* return success password is correct */
		return SUCCESS;
	}
	else
	{
		/* return error password is incorrect */
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
	/* display flashing unlocking on screen while motor is opening the lock */
	while(g_Tick<2)
	{
		LCD_clearScreen();
		_delay_ms(300);
		LCD_displayString("    UnLocking   ");
		_delay_ms(800);
	}
	/* time for opened door */
	while(g_Tick<4)
	{
		/* flag for execute code only one time */
		if(flag==0)
		{
			LCD_clearScreen();
			LCD_displayString(" Door Is Opened ");
			flag=1;
		}
	}
	/* display flashing locking on screen while motor is closing the lock */
	while(g_Tick<6)
	{
		LCD_clearScreen();
		_delay_ms(300);
		LCD_displayString("     Locking    ");
		_delay_ms(800);
	}
	/* Disable timer1 */
	TIMER_deInit(TIMER1);
	/*clear g_Tick*/
	g_Tick=0;
}

void TimerCallBack(void)
{
	g_Tick++; /* Increment flag every timer Interrupt */
}

void ActivateAlarm(void)
{
	g_AlarmCounter++;
	/* configuration of timer */
	T_Config config={TIMER1,CTC,T_CLK_1024,58593};
	/* set call back function for timer 1 */
	TIMER_setCallBack(TIMER1,TimerCallBack);
	/* initialize timer  */
	TIMER_init(&config);
	/* wait for timer */
	while(g_Tick !=8)
	{
		/* flashing error message on screen */
		LCD_clearScreen();
		_delay_ms(300);
		LCD_displayString(" ALARM ACTIVATED ");
		_delay_ms(800);
	}
	/* Disable Timer */
	TIMER_deInit(TIMER1);
	/* Clear g_tick */
	g_Tick=0;
	/* clear g_WrongAttemp */
	g_WrongAttemp=0;
}
