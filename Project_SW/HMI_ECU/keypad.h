/******************************************************************
 * Module     :keypad                                             *
 * Filename   :keypad.h                                           *
 * Description:Header file for keypad driver                      *
 * Created on: Sep 10, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

#ifndef KEYPAD_H_
#define KEYPAD_H_
/******************************************************************
 *                      Includes                                  *
 ******************************************************************/
#include "stdtypes.h"
#include "common_macros.h"
#include <avr/io.h>

/******************************************************************
 *                Definition and Configurations                   *
 ******************************************************************/

/*keyPad configurations for number of rows and */

#define N_col 4
#define N_row 4

/*keyPad configurations for port*/

#define KEYPAD_PORT_DIR  DDRA
#define KEYPAD_PORT_OUT  PORTA
#define KEYPAD_PORT_IN   PINA



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


/*
 * Function responsible for getting the pressed keypad key
 */
uint8 KeyPad_getPressedKey(void);



#endif /* KEYPAD_H_ */
