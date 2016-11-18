/**
 * @file ds18b20.h
 *
 * Driver for the <a href="https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html">Maxim DS18B20</a> one-wire serial temperature sensor.
 *
 * Implements initialization, start a measurement, and read in a measurement to populate three banks of {@link temp_t} objects.
 * @author Jesse Cureton
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "config.h"

/**
 * This is the global-scope temperature array.
 *
 * Data is indexed as temps[{@link BATTERY_BANKS} value][{@link BATTERY_CELLS} value].
 * The single source of truth for anything with regard to a cell temperature. */
temp_t temps[NUM_BANKS+1][12];
uint8_t NUM_TEMPS[NUM_BANKS+1];		///< The number of sensors discovered in a bank, indexed as NUM_TEMPS[{@link BATTERY_BANKS} value].

//Function prototypes
void ds18b20_init();
void ds18b20ReadIn();
void ds18b20StartMeasure();

//Scratchpad values at powerup
#define	ALARM_HIGH			(uint8_t) 0x50
#define ALARM_LOW			(uint8_t) 0x00
#define CONFIG_REG			(uint8_t) 0x3F		//3F = 10bit data, 1F = 9bit

//ROM Commands
#define SEARCH_ROM			(uint8_t) 0xF0
#define MATCH_ROM			(uint8_t) 0x55
#define SKIP_ROM			(uint8_t) 0xCC
#define ALARM_SEARCH		(uint8_t) 0xEC

//Function Commands
#define CONVERT_T			(uint8_t) 0x44
#define WRITE_SCRATCHPAD	(uint8_t) 0x4E
#define READ_SCRATCHPAD		(uint8_t) 0xBE
#define COPY_SCRATCHPAD		(uint8_t) 0x48

#endif /* DS18B20_H_ */
