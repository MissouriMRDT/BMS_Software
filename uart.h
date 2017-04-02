/*
 * uart.h
 *
 *  Created on: Feb 18, 2017
 *      Author: Joseph Hall
 */

#include "msp432p401r.h"

#define TARGET_PB 0x00 //Powerboard
#define TARGET_IND 0x01 //Indicator board

//Port 3
#define PB_RX BIT2
#define PB_TX BIT3

//Port 9
#define IND_RX BIT6
#define IND_TX BIT7

void uart_init();

void uart_tx(uint8_t target, uint8_t data);


