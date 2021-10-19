/******************************************************************
 * Module     :I2C                                                *
 * Filename   :i2c.c                                              *
 * Description:source file for I2C Driver                         *
 * Created on: Sep 26, 2019                                       *
 * Author: keroles girges                                         *
 ******************************************************************/

/******************************************************************
 *                                    Includes                    *
 ******************************************************************/
#include "i2c.h"


/******************************************************************
 *                                 Global Variables               *
 ******************************************************************/



/******************************************************************
 *                    Functions Prototypes (private)              *
 ******************************************************************/



/******************************************************************
 *                                 Functions Definitions          *
 ******************************************************************/
void TWI_init(void)
{
	/* Adjust bit to be 400Khz ,no pre-scaler and CPU 8Mhz */
	TWBR = 2;
	TWSR = 0;  /* NO Pre-Scaler */

	/* Give Micro Address to be 0x01 (used by other devices to call me )
	 * bit 7:1 TWAR
	 */
	TWAR=(0X01<1);

	/* Enable module */
	SET_BIT(TWCR,TWEN);
}

void TWI_start(void)
{
	/* Clear the flag and enable module TWEN and set TWI Start condition TWSTA
	 * clear flag TWINT by writing 1 (must cleared by software)
	 */
	TWCR = 0XA4;
	/* Wait until flag TWINT is set (start bit is send successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

void TWI_stop(void)
{
	/* Clear the flag and enable module TWEN and set TWI Stop condition TWSTO
	 * clear flag TWINT by writing 1 (must cleared by software)
	 */
	TWCR = 0X94;
	/* NO Need to Wait the Flag to be 1 as it is the STOP Bit */
}

void TWI_write(uint8 data)
{
	/* Send the new data */
	TWDR = data;
	/* Clear the flag TWINT before send new data and enable module TWEN */
	TWCR = 0X84;
	/* Wait until flag TWINT is set (New Data is send successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

uint8 TWI_readWithACK(void)
{
	/* Clear the flag TWINT and Enable the module TWEN
	 * Enable receive new data with ACK TWEA
	 */
	TWCR = 0XC4;
	/* Wait until flag TWINT is set (New Data is Received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
	/*read data */
	return TWDR;
}

uint8 TWI_readWithNACK(void)
{
	/* Clear the flag TWINT and Enable the module TWEN
	 * Enable receive new data with NACK (Disable TWEA)
	 */
	TWCR = 0X84;
	/* Wait until flag TWINT is set (New Data is Received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
	/*read data */
	return TWDR;
}

uint8 TWI_getStatus(void)
{
	uint8 status;
	/* read Status with mask prescaler bits */
	status=TWSR & 0XF8;
	return(status);
}


