/******************************************************************
 * Module     :LCD                                                *
 * Filename   :lcd.h                                              *
 * Description:Header file for LCD                                *
 * Created on: Sep 11, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

#ifndef LCD_H_
#define LCD_H_
/******************************************************************
 *                      Includes                                  *
 ******************************************************************/

#include "common_macros.h"
#include "stdtypes.h"
#include <avr/io.h>
#include <util/delay.h>

/******************************************************************
 *                Definition and Configurations                   *
 ******************************************************************/

/* LCD Mode */
#define KEYPAD_BITS_MODE 8       /* Mode of sending data by 4 bits at once or 8 bits */

#if(KEYPAD_BITS_MODE == 4)
/*for 4 bit mode 0 for the first 4 pins and 1 for the last 4 pins */

   #define  PORT_PINS_SELECTION 1
#endif


/*Configuration of LCD Data port*/

#define LCD_DATA_PORT_DIR          DDRC
#define LCD_DATA_PORT_OUT          PORTC
#define LCD_DATA_PORT_IN           PINC


/*Configuration of LCD control pins*/

#define LCD_CTRL_PORT_DIR          DDRD
#define LCD_CTRL_PORT_OUT          PORTD
#define LCD_CTRL_PORT_IN           PIND


#define RS                         PD5
#define RW                         PD6
#define E                          PD7

/*LCD Commands */

#define CLEAR_COMMAND                   0X01
#define TWO_LINE_LCD_EIGHT_BIT_MODE     0X38
#define TWO_LINE_LCD_FOUR_BIT_MODE      0X28
#define FOUR_BITS_DATA_MODE             0x02
#define CURSOR_OFF                      0X0C
#define CURSOR_ON                       0X0E
#define SET_CURSOR_LOCATION             0X80




/******************************************************************
 *                Extern and Global Variables                     *
 ******************************************************************/

/******************************************************************
 *                 Structure and Unions                           *
 ******************************************************************/

/******************************************************************
 *                           Enums                                *
 ******************************************************************/

/******************************************************************
 *                           Macros                               *
 ******************************************************************/

/******************************************************************
 *                    Function Declarations (public)              *
 ******************************************************************/

void LCD_sendCommand(uint8 command);
void LCD_displayCharacter(uint8 data);
void LCD_displayString(const char *str);
void LCD_displayInteger(sint32 num);
void LCD_goToRowColumn(uint8 row,uint8 col);
void LCD_clearScreen(void);
void LCD_init(void);
void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *str);



#endif /* LCD_H_ */
