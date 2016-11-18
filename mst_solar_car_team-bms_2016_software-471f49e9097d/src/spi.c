/**
 * @file spi.c
 *
 * Driver for the MSP430F5529's SPI peripheral.
 *
 * @author Jesse Cureton
 */

#include <msp430.h>
#include "spi.h"
#include "datatypes.h"

/**
 * Initialize the given SPI bus.
 *
 * @param busId One of the board's {@link SPI_BUSSES SPI busses}.
 */
void spi_init(uint8_t busId)
{
	switch(busId)
	{
		case LTC_BUS:
			UCA0CTL1  = UCSWRST;	//Hold the device in a reset state while we configure
			UCA0CTL0  = 0x69;		//SPI mode 11, MSB first, 8 bit data, 3pin master synchronous mode
			UCA0CTL1 |= 0xC0;		//SPI clock (BRCLK) source = SMCLK
			UCA0BR1	  = 0x00;		//Set the high bit of the baud rate generator
			UCA0BR0	  = 0x14;		//Set the low bit of the baud rate generator (SMCLK / 20 == 1MHz SPI)
			//UCA0IE   |= 0x04;		//Enable interrupts
			UCA0CTL1 &= ~UCSWRST;	//Release the bus from reset state
			break;
		case REG_BUS:
			UCB0CTL1  = UCSWRST;	//Hold the device in a reset state while we configure
			UCB0CTL0  = 0x69;		//SPI mode 11, MSB first, 8 bit data, 3pin master synchronous mode
			UCB0CTL1 |= 0xC0;		//SPI clock (BRCLK) source = SMCLK
			UCB0BR1	  = 0x00;		//Set the high bit of the baud rate generator
			UCB0BR0	  = 0x14;		//Set the low bit of the baud rate generator (SMCLK / 20 == 1MHz SPI)
			//UCB0IE   |= 0x04;		//Enable interrupts
			UCB0CTL1 &= ~UCSWRST;	//Release the bus from reset state
			break;
		default:
			break;
	}
}

/**
 * Begin a SPI transaction.
 *
 * @param busId One of the board's {@link SPI_BUSSES SPI busses}.
 * @param data Data to send over SPI.
 * @return Data received during the SPI transaction.
 */
uint8_t spi_tx(uint8_t busId, uint8_t data)
{
	switch(busId)
	{
		case LTC_BUS:
			while((UCA0STAT & UCBUSY) != 0);
			UCA0TXBUF = data;
			while((UCA0STAT & UCBUSY) != 0);
			return UCA0RXBUF;
			break;
		case REG_BUS:
		default:
			while((UCB0STAT & UCBUSY) != 0);	//Wait for the transmit buffer to be empty
			UCB0TXBUF = data;	//Set the data to be transmit in the TX buffer
			while((UCB0STAT & UCBUSY) != 0);	//Wait for the rx buffer to be full
			return UCB0RXBUF;	//Return the RX buffer
	}
}


