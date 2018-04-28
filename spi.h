/*
 * spi.h
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#ifndef SPI_H_
#define SPI_H_
#include "msp432p401r.h"
#include <gpio.h>

//Port 10
#define BMS_SCLK    GPIO_PIN1
#define BMS_MOSI    GPIO_PIN2
#define BMS_MISO    GPIO_PIN3

void spi_init();

uint8_t spi_tx(uint8_t data);

#endif /* SPI_H_ */
