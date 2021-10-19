/******************************************************************
 * Module     :Timer v                                            *
 * Filename   :timer.c                                            *
 * Description:source file for Timer Driver                       *
 * Created on: Oct 01, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

/******************************************************************
 *                                    Includes                    *
 ******************************************************************/
#include "timer.h"


/******************************************************************
 *                                 Global Variables               *
 ******************************************************************/

/* Global variable to hold pointer of call back function */

static volatile void(*g_callBackPtr0)(void)=NULL_PTR;      /* Call back function of timer 0 */
static volatile void(*g_callBackPtr1)(void)=NULL_PTR;      /* Call back function of timer 1 */
static volatile void(*g_callBackPtr2)(void)=NULL_PTR;      /* Call back function of timer 2 */


/******************************************************************
 *                    Functions Prototypes (private)              *
 ******************************************************************/


/*******************************************************************
 *                   Interrupt Service Routines                    *
 *******************************************************************/

/************************TIMER0*************************************/
ISR(TIMER0_OVF_vect)  /* ISR of Normal Mode */
{
	if(g_callBackPtr0 != NULL_PTR)
	{
		(*g_callBackPtr0)();  /* Call call back function */
	}
}

ISR(TIMER0_COMP_vect)  /* ISR of CTC Mode */
{
	if(g_callBackPtr0 != NULL_PTR)
	{
		(*g_callBackPtr0)();  /* Call call back function */
	}
}


/************************TIMER1*************************************/
ISR(TIMER1_OVF_vect)  /* ISR of Normal Mode */
{
	if(g_callBackPtr1 != NULL_PTR)
	{
		(*g_callBackPtr1)();  /* Call call back function */
	}
}
ISR(TIMER1_COMPA_vect)  /* ISR of CTC Mode */
{
	if(g_callBackPtr1 != NULL_PTR)
	{
		(*g_callBackPtr1)();  /* Call call back function */
	}
}


/************************TIMER2*************************************/
ISR(TIMER2_OVF_vect)  /* ISR of Normal Mode */
{
	if(g_callBackPtr2 != NULL_PTR)
	{
		(*g_callBackPtr2)();  /* Call call back function */
	}
}
ISR(TIMER2_COMP_vect)  /* ISR of CTC Mode */
{
	if(g_callBackPtr2 != NULL_PTR)
	{
		(*g_callBackPtr2)();  /* Call call back function */
	}
}


/******************************************************************
 *                                 Functions Definitions          *
 ******************************************************************/
void TIMER_init(T_Config *config)
{
	switch (config->channel)
	{
	case TIMER0:
		SET_BIT(TCCR0,FOC0);  /* Set FOC0 for Non PWM mode  */
		if(config->mode==NORMAL)  /* configure timer 0 normal mode and enable interrupt and CPU pre-scaler */
		{
			SET_BIT(TIMSK,TOIE0); /*enable interrupt for overflow mode(normal mode )*/
			TCCR0 &= ~0X78;       /* Clear WGM01 WGM00 COM00 COM01 for normal mode */
		}
		else if(config->mode==CTC)
		{
			SET_BIT(TIMSK,OCIE0); /*enable interrupt for CTC mode */
			TCCR0 |= 0X08;        /* WGM00 0 WGM01 1 Normal CTC Mode */
			OCR0 = (uint8)config->compare; /* enter compare value*/
		}
		TCNT0 =0; /* Clear counter register */
		TCCR0 =(TCCR0 &(~0X07)) | (config->clock & 0x07); /* configure Pre-scaler */
		break;


	case TIMER1:
		TCCR1A |= ((1<<FOC1A)| (1<< FOC1B));  /* Set bit FOC1A and FOC1B of Non PWM Mode */
		if(config->mode == NORMAL)
		{
			SET_BIT(TIMSK,TOIE1); /*enable interrupt for overflow mode(normal mode )*/
			/* Configure TIMER1 Normal mode
			 * WGM10:13 =0;
			 * COM All equal zero
			 */
			TCCR1B=0;
			TCCR1A &= ~0XF3;
		}
		else if(config->mode ==CTC)
		{
			SET_BIT(TIMSK,OCIE1A); /*enable interrupt for CTC channel A*/

			/* configure TIMER1 at mode CTC mode 4 i.e WGM12=1 in TCCR1B
			 * COM= 0 normal CTC
			 */
			TCCR1B |= 0X08; /* set bit WGM12 Bit 3*/
			TCCR1A &= ~0XF3;
			OCR1A = config->compare; /* enter compare value */
		}
		TCNT1=0;    /* Clear Counter 1 */
		TCCR1B =(TCCR1B &(~0X07)) | (config->clock & 0x07); /* configure Pre-scaler */
		break;

	case TIMER2:
	    SET_BIT(TCCR2,FOC2);  /* Set FOC2 for Non PWM mode  */
	    if(config->mode==NORMAL)  /* configure timer 2 normal mode and enable interrupt and CPU pre-scaler */
	      {
		    SET_BIT(TIMSK,TOIE2); /*enable interrupt for overflow mode(normal mode )*/
		    TCCR2 &= ~0X78;       /* Clear WGM21 WGM20 COM20 COM21 for normal mode */
	      }
	    else if(config->mode==CTC)
	     {
		   SET_BIT(TIMSK,OCIE2); /*enable interrupt for CTC mode */
		   TCCR2 |= 0X08;        /* WGM20 0 WGM21 1 Normal CTC Mode */
		   OCR2 =(uint8) config->compare; /* enter compare value*/
	     }
	   TCNT2 =0; /* Clear counter register */
	   TCCR2 =(TCCR2 &(~0X07)) | (config->clock & 0x07); /* configure Pre-scaler */
	break;
	}
}


void TIMER_setCallBack(T_Channel channel,void(*a_ptr)(void))
{
	switch (channel)
	{
	case TIMER0:
		g_callBackPtr0 = a_ptr; /* store pointer of set call back function */
		break;
	case TIMER1:
		g_callBackPtr1 = a_ptr;
		break;
	case TIMER2:
		g_callBackPtr2 = a_ptr;
		break;
	}
}

void TIMER_deInit(T_Channel channel)
{
	switch (channel)
	{
	case TIMER0:
		TIMSK &= ~0X03; /* Disable All TIMER0 interrupt */
		TCNT0=0;        /* Disable timer0 */
		TCCR0=0;        /* Clear Counter*/
		OCR0=0;         /*Clear Compare Register*/
		break;
	case TIMER1:
		TIMSK &= ~0X1C; /*Disable All TIMER1 Interrupt */
		TCCR1A=0;       /* Disable Timer1 */
		TCCR1B=0;
		TCNT1=0;        /* Clear Counter*/
		OCR1A=0;        /*Clear Compare Register*/

		break;
	case TIMER2:
		TIMSK &= ~0XC0; /* Disable All TIMER2 interrupt */
		TCNT2=0;        /* Disable TIMER2 */
		TCCR2=0;        /* Clear Counter*/
		OCR0=0;         /*Clear Compare Register*/
		break;
	}
}
