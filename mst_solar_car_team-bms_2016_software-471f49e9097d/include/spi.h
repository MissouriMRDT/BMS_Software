/**
 * @file spi.h
 *
 * Driver for the MSP430F5529's SPI peripheral.
 *
 * @author Jesse Cureton
 */

#ifndef SPI_H_
#define SPI_H_
#include "datatypes.h"

/**
 * @defgroup SPI_BUSSES SPI Busses Available
 *
 * @{
 */
#define LTC_BUS	0x00	///< Isolated SPI bus used to communicate with the {@link ltc6803.c LTC6803} chips.
#define REG_BUS 0x01	///< Standard SPI bus used for all other SPI peripherals on the board.
/** @} */

// Initialize the SPI bus
void spi_init(uint8_t busId);

// Transmit a byte over SPI bus
// SPI is an exchange protocol, so we can't transmit without also recieving
// Just ignore the return if you don't need the value
uint8_t spi_tx(uint8_t busId, uint8_t data);

#endif /* SPI_H_ */
