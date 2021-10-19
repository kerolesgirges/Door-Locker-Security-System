/******************************************************************
 * Module     : Timer.h                                           *
 * Filename   :timer.h                                            *
 * Description:Header file for Timer Driver                       *
 * Created on: Oct 01, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/
#ifndef TIMER_H_
#define TIMER_H_
/******************************************************************
 *                      Includes                                  *
 ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "common_macros.h"
#include "stdtypes.h"

/******************************************************************
 *                Definition and Configurations                   *
 ******************************************************************/
#define NULL_PTR ((void *)0)

/******************************************************************
 *                Extern and Global Variables                     *
 ******************************************************************/

/******************************************************************
 *                           Enums                                *
 ******************************************************************/
/* ENUM for timer 0 1 AND 2 pre-scaler
 * Timer 2 has a different pre-scaler values
 */
typedef enum {
	T_OFF,T_CLK_1,T_CLK_8,T_CLK_64,T_CLK_256,T_CLK_1024,
	T_OFF2=0,T_CLK2_1,T_CLK2_8,T_CLK2_32,T_CLK2_64,T_CLK2_128,T_CLK2_256,T_CLK2_1024 /* Values of Prescaler of timer2 */
}T_Clock;


/* ENUM for timer channel */
typedef enum
{
	TIMER0,TIMER1,TIMER2
}T_Channel;

/* ENUM for timer mode (overflow and CTC) */
typedef enum
{
	NORMAL,CTC
}T_Mode;

/******************************************************************
 *                 Structure and Unions                           *
 ******************************************************************/
/* structure for timer configuration */
typedef struct
{
	T_Channel channel;
	T_Mode mode;
	T_Clock clock;
	uint16 compare;  /* compare value for CTC Mode */
}T_Config;

/******************************************************************
 *                           Macros                               *
 ******************************************************************/

/******************************************************************
 *                    Function Declarations (public)              *
 ******************************************************************/

void TIMER_init(T_Config *config);
void TIMER_setCallBack(T_Channel channel,void(*a_ptr)(void));
void TIMER_deInit(T_Channel channel);



#endif /* TIMER_H_ */
