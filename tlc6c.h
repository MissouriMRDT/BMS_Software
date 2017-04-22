/*
 * tlc6c.h
 *
 *  Created on: Apr 13, 2017
 *      Author: tef
 */

#ifndef TLC6C_H_
#define TLC6C_H_

//Port 8
#define LED_SER_IN      BIT4
#define LED_RCK         BIT5

//Port 9
#define GAUGE_ON        BIT0
#define LED_SRCK        BIT1

void tlc6c_write_bit(int bit);

void tlc6c_write_byte(int byte);

#endif /* TLC6C_H_ */
