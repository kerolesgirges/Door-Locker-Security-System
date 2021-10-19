/******************************************************************
 * Module     :keypad                                             *
 * Filename   :keypad.c                                           *
 * Description:source file for keypad driver                      *
 * Created on: Sep 10, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

/******************************************************************
 *                                    Includes                    *
 ******************************************************************/
#include "keypad.h"


/******************************************************************
 *                                 Global Variables               *
 ******************************************************************/



/******************************************************************
 *                    Functions Prototypes (private)              *
 ******************************************************************/

#if(N_col==4)
/*
 * function responsible for adjust number in keypad to its corresponding functional
 * number
 *
 *    7     8    9   /
 *    4     5    6   *
 *    1     2    3   -
 *    on    0    =   +
 */
static uint8 KeyPad_4x4_adjustKeyNumber(uint8 key_number);


#elif(N_col==3)

/*
 * function responsible for adjust number in keypad to its corresponding functional
 * number
 *     1   2    3
 *     4   5    6
 *     7   8    9
 *     *   0    #
 */
static uint8 KeyPad_4x3_adjustKeyNumber(uint8 key_number);
#endif

/******************************************************************
 *                                 Functions Definitions          *
 ******************************************************************/

uint8 KeyPad_getPressedKey(void)
{
	uint8 col,row;
	while(1)
	{
	  for(col=0;col<N_col;col++)
	  {
		  /*every time 1 columns output and others pins is input including rows  */
		  KEYPAD_PORT_DIR=(0X10<<col);
		  /* clear output pin at this columns and activate pull up resistor for others pins   */
		  KEYPAD_PORT_OUT=(~(0X10<<col));

		  /* loop the rows to check if any key is pressed   */
		  for(row=0;row<N_row;row++)
		  {
			  if(BIT_IS_CLEAR(KEYPAD_PORT_IN,row)) /*if the key is pressed (negative logic _ pull up connection)  */
			  {
                   #if(N_col==4)
				  return KeyPad_4x4_adjustKeyNumber((row*N_col)+col+1);
                   #elif(N_col==3)
				  return KeyPad_4x3_adjustKeyNumber((row*N_col)+col+1);
                   #endif
			  }
		  }
	  }

  }
}


#if(N_col==4)

static uint8 KeyPad_4x4_adjustKeyNumber(uint8 key_number)
{
	switch (key_number)
	{
	     case 1:
	    	 return 7;
	     case 2:
	    	 return 8;
	     case 3:
	    	 return 9;
	     case 4:
	    	 return '/';
	     case 5:
	    	 return 4;
	     case 6:
	    	 return 5;
	     case 7:
	    	 return 6;
	     case 8:
	    	 return '*';
	     case 9:
	    	 return 1;
	     case 10:
	    	 return 2;
	     case 11:
	    	 return 3;
	     case 12:
	    	 return '-';
	     case 13:
	    	 return 13;
	     case 14:
	    	 return 0;
	     case 15:
	    	 return '=';
	     case 16:
	    	 return '+';
	     default :
	    	 return key_number;
	}
}


#elif(N_col==3)

static uint8 KeyPad_4x3_adjustKeyNumber(uint8 key_number)
{
	switch(key_number)
	{
	case 10:
		return'*';
	case 11:
		return 0;
	case 12:
		return '#';
	default:
		return key_number;
	}
}


#endif
