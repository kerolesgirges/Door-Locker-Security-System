/******************************************************************
 * Module     : UART                                              *
 * Filename   :uart.c                                             *
 * Description:source file for USART DRIVER                       *
 * Created on: Sep 18, 2019                                       *
* Author: keroles girges                                         *
 ******************************************************************/

/******************************************************************
 *                                    Includes                    *
 ******************************************************************/
#include "uart.h"


/*********************** Baud Prescaler ***************************/

#define BAUD_PRESCALER ((F_CPU/(8*UART_BAUDRATE))-1)


/******************************************************************
 *                                 Global Variables               *
 ******************************************************************/



/******************************************************************
 *                    Functions Prototypes (private)              *
 ******************************************************************/



/******************************************************************
 *                                 Functions Definitions          *
 ******************************************************************/
void UART_init(void)
{
	/* U2X is set for Double Transmission */
	UCSRA |= 0X02;

	/****USART Control and Status Register B *********************
	 * RXCIE =0  Rx Complete Interrupt Enable  (Disabled)
	 * TXCIE =0  Tx Complete Interrupt Enable  (Disabled)
	 * UDRIE =0  UART Data Register Empty Interrupt (Disabled)
	 * RXEN =1   Receiver Enable
	 * TXEN =1   Transmitter Enable
	 * UCSZ2 = 0 8-Bits Data (shared with UCSZ 1:0 in UCSRC Reg
	 * RXB8  Receive bit 8 (the 9th bit if the data is 9-bit)
	 * TXB8  Transmit bit 8 (the 9th bit if the data is 9-bit)
	 */
	UCSRB |= 0X18; /* Bit 4 and 3 */

	/****USART Control and Status Register C *********************
	 * URSEL=1 UART Register Select 1 to write to UCSRC
	 * UMSEL=0 Asynchronous mode
	 * UPM1:0 =00 Disable Parity
	 * USBS =0 (1) STOP Bit
	 * UCSZ1:0 =11 For 8bit Character size
	 * UCPOL =0 (Used with Synchronous Mode)
	 */
	UCSRC |= 0X86;

	/* Baud Rate 9600 at 1 MHz CPU clock and Double data rate */

	UBRRH = (BAUD_PRESCALER>>8);
	UBRRL = BAUD_PRESCALER;
}

void UART_sendByte(const uint8 data)
{
	/* UDR data register is empty and ready to receive data
	 * when TXC or UDRE flag is SET (1)
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE));
	/* Put Data in UDR Register it also clear the flag */
	UDR=data;
}

uint8 UART_receiveByte(void)
{
	/* Wait until the data is received and
	 * RXC Flag is set
	 */
	while(BIT_IS_CLEAR(UCSRA,RXC));
	/* Read data in UDR the flag will be Cleared */
	return UDR;
}


void UART_sendString(const uint8 *str)
{
	uint8 i=0;
	while(str[i]!='\0')
	{
		UART_sendByte(str[i]);
		i++;
	}
}

void UART_receiveString(uint8 *str)
{
	uint8 i=0;
	do
	{
		str[i]=UART_receiveByte();
		i++;
	}while(str[i-1] != '#');
	str[i-1]='\0'; /* Replace # with NULL character */
}
