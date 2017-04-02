/*
 * ds18b20.h
 *
 *  Created on: Mar 27, 2017
 *      Author: tef
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "msp.h"
#include "msp432p401r.h"
#include "onewire.h"
#include "delay.h"


//These commands are given in the DS18B20 datasheet on pages 10-12.

//Config resolution
#define RES_9 0x1F
#define RES_10 0x3F
#define RES_11 0x5F
#define RES_12 0x7F

onewire_t ow_temp;

//TODO: Add code for multiple temp sensors
//void init_multidrop();
//void write_scratch_multidrop();
//void read_scratch_multidrop();

void write_scratch_singledrop(onewire_t *ow, uint8_t t_lo, uint8_t t_hi, uint8_t res);

void start_conv_singledrop(onewire_t *ow);

float read_scratch_singledrop(onewire_t *ow);

uint8_t dallas_crc8(uint8_t *data, int size); //size in bytes


#endif /* DS18B20_H_ */
