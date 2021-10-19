/******************************************************************
 * Module     : External EEPROM                                   *
 * Filename   :external eeprom                                    *
 * Description:source file for External EEPROM                    *
 * Created on: Sep 26, 2019                                       *
* Author: keroles girges                                         *
 ******************************************************************/

/******************************************************************
 *                                    Includes                    *
 ******************************************************************/
#include"i2c.h"
#include"external_eeprom.h"


/******************************************************************
 *                                 Global Variables               *
 ******************************************************************/



/******************************************************************
 *                    Functions Prototypes (private)              *
 ******************************************************************/



/******************************************************************
 *                                 Functions Definitions          *
 ******************************************************************/
void EEPROM_init(void)
{
	/* JUST initialize I2C */
	TWI_init();
}

uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data)
{
	/* Send Start bit */
	TWI_start();
	/* check status register if the start bit send successfully */
	if(TWI_getStatus() != TW_START)
		return ERROR;
	/* Send address of EEPROM and The the first 3 bit of the required location
	 * address and R/W bit (0 for write )
	 * EEPROM Address is 0XA0
	 */
	TWI_write((uint8)((0xA0 | (u16addr & 0x0700)>>7) | 0));
	/* Check status register if the Address is sent and received ACK */
	if(TWI_getStatus() != TW_MT_SLA_W_ACK)
		return ERROR;
	/* Send the the rest of required location address */
	TWI_write((uint8) (u16addr));
	/* Check status register if the data is sent and received ACK */
	if(TWI_getStatus() != TW_MT_DATA_ACK)
		return ERROR;
	/* send the required data to EEPROM */
	TWI_write(u8data);
	/* Check status register if the data is sent and received ACK */
	if(TWI_getStatus() != TW_MT_DATA_ACK)
		return ERROR;
	/* send STOP Bit */
	TWI_stop();

	return SUCCESS;
}

uint8 EEPROM_readByte(uint16 u16addr,uint8 *u8data)
{
	/* Send Start Bit */
	TWI_start();
	/* Check if start bit is sent successfully */
	if(TWI_getStatus() != TW_START)
		return ERROR;

	/* send the device address and the 3 bit A10 TO A9
	 * of the required memory location and R/W bit(write 0)
	 */
	TWI_write((uint8)((0xA0 | (u16addr & 0x0700)>>7) | 0));
	/* check of the address is send and received ACK */
	if(TWI_getStatus() != TW_MT_SLA_W_ACK)
		return ERROR;

	/* send the rest of required memory location address */
	TWI_write((uint8)(u16addr));
	/* Check if the data is send successfully and received ACK */
	if(TWI_getStatus() != TW_MT_DATA_ACK)
		return ERROR;

	/* Re-send the START Bit */
	TWI_start();
	/* check for repeated start */
	if(TWI_getStatus() != TW_REP_START)
		return ERROR;

	/* send the device address with read */
	TWI_write((uint8)((0xA0 | (u16addr & 0x0700)>>7) | 1));
	/* check if the address is send and received ACK */
	if(TWI_getStatus() != TW_MT_SLA_R_ACK)
		return ERROR;

	/* start read data and send negative ACK */
	*u8data = TWI_readWithNACK();
	/* check if data is received and send a negative ACK */
	if(TWI_getStatus() != TW_MR_DATA_NACK)
		return ERROR;
    /* Send the Stop Bit */
    TWI_stop();

	return SUCCESS;
}
