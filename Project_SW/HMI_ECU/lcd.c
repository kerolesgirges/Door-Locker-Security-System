/******************************************************************
 * Module     :LCD                                                *
 * Filename   :lcd.c                                              *
 * Description:source file for LCD driver                         *
 * Created on: Sep 11, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

/******************************************************************
 *                                    Includes                    *
 ******************************************************************/
#include "lcd.h"
#include <stdlib.h>


/******************************************************************
 *                                 Global Variables               *
 ******************************************************************/



/******************************************************************
 *                    Functions Prototypes (private)              *
 ******************************************************************/



/******************************************************************
 *                                 Functions Definitions          *
 ******************************************************************/

void LCD_init(void)
{
	LCD_CTRL_PORT_DIR |= (1<<RS) | (1<<RW) | (1<<E);        /* Configure control pin RW & RS & E as output  */
#if(KEYPAD_BITS_MODE == 4)
 #if(PORT_PINS_SELECTION  == 0)
	LCD_DATA_PORT_DIR |= 0X0F;                              /* Configure the first 4 pins as output */
 #elif(PORT_PINS_SELECTION  == 1)
	LCD_DATA_PORT_DIR |= 0XF0;                              /* Configure the last 4 pins as output */
 #endif
	LCD_sendCommand(FOUR_BITS_DATA_MODE);                   /* initialize LCD in 4_bit data mode */
	LCD_sendCommand(TWO_LINE_LCD_FOUR_BIT_MODE);            /* use 2-line LCD + 4-bit Data Mode + 5*7 dot display Mode */
#elif (KEYPAD_BITS_MODE == 8)
	LCD_DATA_PORT_DIR |=0XFF;                               /* Configure data port as output  */
	LCD_sendCommand(TWO_LINE_LCD_EIGHT_BIT_MODE);           /* use 2-line LCD + 8-bit Data Mode + 5*7 dot display Mode */
#endif
	LCD_sendCommand(CURSOR_OFF);
	LCD_sendCommand(CLEAR_COMMAND);                         /* clear LCD at the beginning */
}



void LCD_sendCommand(uint8 command)
{
	CLEAR_BIT(LCD_CTRL_PORT_OUT,RS);       /* Clear RS for sending command */
	CLEAR_BIT(LCD_CTRL_PORT_OUT,RW);       /* Clear RW for Writing */
	_delay_us(1);                          /* Delay tas(address set up time tas= 50 ns minimum)  */
	SET_BIT(LCD_CTRL_PORT_OUT,E);          /* Set E */
	_delay_us(1);                          /* Delay (tpw-tdsw = 290 ns -100 ns =190ns) minimum */
#if (KEYPAD_BITS_MODE == 4)
/* send the higher 4 bit of command first */
  #if (PORT_PINS_SELECTION  == 0)
	 LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0X0F)) |((command & 0XF0) >> 4);
  #elif (PORT_PINS_SELECTION  == 1)
	 LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0XF0)) | (command & 0XF0);
  #endif
     _delay_us(1);                          /* Delay tdsw =100 ns minimum */
	 CLEAR_BIT(LCD_CTRL_PORT_OUT,E);        /* Clear E */
	 _delay_us(1);                          /* Delay th(hold time) = 13 ns minimum */
	 SET_BIT(LCD_CTRL_PORT_OUT,E);          /* Set E */
	 _delay_us(1);                          /* Delay (tpw-tdsw = 290 ns -100 ns =190ns) minimum */
/* send the lower 4 bit of command */
  #if (PORT_PINS_SELECTION  == 0)
     LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0X0F)) |((command & 0X0F));
  #elif (PORT_PINS_SELECTION  == 1)
     LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0XF0)) | ((command & 0X0F) <<4);
  #endif
#elif(KEYPAD_BITS_MODE == 8)
	 LCD_DATA_PORT_OUT = command;           /* Write command to output port */
#endif
	_delay_us(1);                          /* Delay tdsw =100 ns minimum */
	CLEAR_BIT(LCD_CTRL_PORT_OUT,E);        /* Clear E */
	_delay_us(1);                          /* Delay th(hold time) = 13 ns minimum */
}

void LCD_displayCharacter(uint8 data)
{
	SET_BIT(LCD_CTRL_PORT_OUT,RS);         /* Clear RS for sending data */
	CLEAR_BIT(LCD_CTRL_PORT_OUT,RW);       /* Clear RW for Writing */
	_delay_us(1);                          /* Delay tas(address set up time tas= 50 ns minimum)  */
	SET_BIT(LCD_CTRL_PORT_OUT,E);          /* Set E */
	_delay_us(1);                          /* Delay (tpw-tdsw = 290 ns -100 ns =190ns) minimum */
#if (KEYPAD_BITS_MODE == 4)
/* send the higher 4 bit of data first */
  #if (PORT_PINS_SELECTION  == 0)
	 LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0X0F)) |((data & 0XF0) >> 4);
  #elif (PORT_PINS_SELECTION  == 1)
	 LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0XF0)) | (data & 0XF0);
  #endif
     _delay_us(1);                          /* Delay tdsw =100 ns minimum */
	 CLEAR_BIT(LCD_CTRL_PORT_OUT,E);        /* Clear E */
	 _delay_us(1);                          /* Delay th(hold time) = 13 ns minimum */
	 SET_BIT(LCD_CTRL_PORT_OUT,E);          /* Set E */
	 _delay_us(1);                          /* Delay (tpw-tdsw = 290 ns -100 ns =190ns) minimum */
/* send the lower 4 bit of data */
  #if (PORT_PINS_SELECTION  == 0)
     LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0X0F)) |((data & 0X0F));
  #elif (PORT_PINS_SELECTION  == 1)
     LCD_DATA_PORT_OUT = (LCD_DATA_PORT_OUT & (~ 0XF0)) | ((data & 0X0F) <<4);
  #endif
#elif(KEYPAD_BITS_MODE == 8)
	 LCD_DATA_PORT_OUT = data;             /* Write data to output port */
#endif
	_delay_us(1);                          /* Delay tdsw =100 ns minimum */
	CLEAR_BIT(LCD_CTRL_PORT_OUT,E);        /* Clear E */
	_delay_us(1);                          /* Delay th(hold time) = 13 ns minimum */
}

void LCD_displayString(const char *str)
{
	uint8 i=0;
	while(str[i]!='\0')
	{
		LCD_displayCharacter(str[i]);
		i++;
	}
}


void LCD_displayInteger(sint32 num)
{
	char buff[16];
	itoa(num,buff,10);             /*function to convert decimal number to ASCII code and store it in buff  */
	LCD_displayString(buff);       /*Display string */
}

void LCD_goToRowColumn(uint8 row,uint8 col)
{
	uint8 address;
	/* Calculate the address of cursor   */
	switch(row)
	{
	  case 0:
		  address=0x00+col;
		  break;
	  case 1:
		  address=0x40+col;
		  break;
	  case 2:
		  address=0x10+col;
		  break;
	  case 3:
		  address=0x50+col;
		  break;
	  default:
		  address=0x00+col;  /* row 0 if row number is wrong  */
	}
	LCD_sendCommand(SET_CURSOR_LOCATION | address); /*send command to move cursor to required position */
}


void LCD_clearScreen(void)
{
	LCD_sendCommand(CLEAR_COMMAND);
}

void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *str)
{
	LCD_goToRowColumn(row,col);   /* go to required position */
	LCD_displayString(str);       /* display string   */
}
