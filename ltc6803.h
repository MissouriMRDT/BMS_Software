/*
 * ltc6803.h
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#ifndef LTC6803_H_
#define LTC6803_H_
#include <gpio.h>
#include "msp432p401r.h"
#include "spi.h"
#include "uart.h"

#define RESET_PEC                       pecVAL = 0x41  ///< Reset the PEC value to its initial value, required call before calling
//LTC command values
#define WRCFG                           0x01    ///< Write configuration register group
#define RDCFG                           0x02    ///< Read configuration register group
#define RDCV                            0x04    ///< Read all cell voltages
#define RDFLG                           0x0C    ///< Read flag register group
#define STCVAD                          0x10    ///< Start cell voltage conversion - all cells
#define STCVDC                          0x60    ///< Start cell voltage conversion - discharge enabled
#define STCCLR                          0x1D    ///< Clear cell voltage registers
#define PLADC                           0x40    ///< Poll ADC status
#define PLINT                           0x50    ///< Poll interrupt status
#define DAGN                            0x52    ///< Start diagnose and poll status
#define RDDGNR                          0x54    ///< Read diagnostic register

//PEC values for LTC commands.
#define WRCFG_PEC                       0xC7    ///< Write configuration register group
#define RDCFG_PEC                       0xCE    ///< Read configuration register group
#define RDCV_PEC                        0xDC    ///< Read all cell voltages
#define RDFLG_PEC                       0xE4    ///< Read flag register group
#define STCVAD_PEC                      0xB0    ///< Start cell voltage conversion - all cells
#define STCVDC_PEC                      0xE7    ///< Start cell voltage conversion - discharge enabled
#define STCCLR_PEC                      0x93    ///< Clear cell votlage registers
#define PLADC_PEC                       0x07    ///< Poll ADC status
#define PLINT_PEC                       0x77    ///< Poll interrupt status
#define DAGN_PEC                        0x79    ///< Start diagnose and poll status
#define RDDGNR_PEC                      0x6B    ///< Read diagnostic register
#define CDC0                            0x00    ///< Standby mode. No measurements taken.
#define CDC1                            0x01    ///< Comparator off; measurements taken at request of SPI master and complete after 13ms.

//Port 10
#define BMS_CSBI    GPIO_PIN0

uint8_t pecVAL;
uint8_t pecIN;
uint8_t cell_regs[16];

void ltc6803_pec(uint8_t val);

void ltc6803_wrcfg(uint8_t cdc_mode);

void ltc6803_stcvad();

void ltc6803_rdcv();

void ltc6803_dagn();

void ltc6803_rddgnr();


#endif /* LTC6803_H_ */
