/*
 * spi.h
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#ifndef SPI_H_
#define SPI_H_
#include "msp432p401r.h"

//Port 1
#define BMS_SCLK BIT5
#define BMS_MOSI BIT6
#define BMS_MISO BIT7

void spi_init();

uint8_t spi_tx(uint8_t data);

#endif /* SPI_H_ */
