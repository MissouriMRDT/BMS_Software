/*
 * spi.h
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#ifndef SPI_H_
#define SPI_H_
#include "msp432p401r.h"

//Port 10
#define BMS_SCLK BIT1
#define BMS_MOSI BIT2
#define BMS_MISO BIT3

void spi_init();

uint8_t spi_tx(uint8_t data);

#endif /* SPI_H_ */
