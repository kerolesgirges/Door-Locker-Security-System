/******************************************************************
 * Module     : I2C                                               *
 * Filename   :i2c.h                                              *
 * Description:Header file for I2C Driver                         *
 * Created on: Sep 26, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

#ifndef I2C_H_
#define I2C_H_
/******************************************************************
 *                      Includes                                  *
 ******************************************************************/
#include "stdtypes.h"
#include "common_macros.h"
#include <avr/io.h>

/******************************************************************
 *                Definition and Configurations                   *
 ******************************************************************/
/* I2C Status Bits in the TWSR Register */
#define TW_START         0x08 // start has been sent
#define TW_REP_START     0x10 // repeated start
#define TW_MT_SLA_W_ACK  0x18 // Master transmit ( slave address + Write request ) to slave + Ack received from slave
#define TW_MT_SLA_R_ACK  0x40 // Master transmit ( slave address + Read request ) to slave + Ack received from slave
#define TW_MT_DATA_ACK   0x28 // Master transmit data and ACK has been received from Slave.
#define TW_MR_DATA_ACK   0x50 // Master received data and send ACK to slave
#define TW_MR_DATA_NACK  0x58 // Master received data but doesn't send ACK to slave

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
void TWI_init(void);
void TWI_start(void);
void TWI_stop(void);
void TWI_write(uint8 data);
uint8 TWI_readWithACK(void);
uint8 TWI_readWithNACK(void);
uint8 TWI_getStatus(void);



#endif /* I2C_H_ */
