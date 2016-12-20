/**
 * @file ds18b20.c
 *
 * Driver for the <a href="https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html">Maxim DS18B20</a> one-wire serial temperature sensor.
 *
 * Implements initialization, start a measurement, and read in a measurement to populate three banks of {@link temp_t} objects.
 * @author Jesse Cureton
 */
 /*
#include "msp430.h"
#include "main.h"
#include "datatypes.h"
#include "onewire.h"
#include "pins.h"
#include "can.h"

#include "drivers/ds18b20.h"
*/

#include "bms.h"
#include "onewire.h"
#include "ds18b20.h"


/**
 * Initialize up to three banks of DS18B20 temperature sensors.
 *
 * This will populate the {@link temps} array with all of the temperature sensors in our banks.
 *
 * @see temp_t
 */
void ds18b20_init()
{
	for(int8_t bus = 2; bus >= 1; bus--)
	{
		//Device scan
		uint8_t devPresent = 0; 										//Indicator to see if a device is present
		NUM_TEMPS[bus] = 0;											//Number of sensors in each bus is zero to start with

		devPresent = owFindFirst(bus);
		while(devPresent)
		{
			TempAddr[bus]; = curROMAddr;				//Set the ds18b20 object in the numtemps[bus] position of bus to current address
			NUM_TEMPS[bus]++;											//Increment the number of sensors discovered in this bus
			devPresent = owFindNext(bus);								//Find the next device on the bus
		}

		//Configure all the registers on the device properly
		owReset(bus);
		owWriteByte(bus, SKIP_ROM);									//Address all devices on the bus
		owWriteByte(bus, WRITE_SCRATCHPAD);							//Tell them we'll be writing to the scratchpad
		owWriteByte(bus, ALARM_HIGH);									//Configure high temp alarm
		owWriteByte(bus, ALARM_LOW);									//Configure low temp alarm
		owWriteByte(bus, CONFIG_REG);									//Configure to 10bit resolution

		//if(NUM_TEMPS[bus] < NUM_CELLS[bus]) ERRORS |= TEMP_FAULT;		//Make sure we have at least as many temperature sensors as we do cells
	}
}

/**
 * Begins a remote temperature measurement on the three banks of DS18B20 temperature sensors.
 *
 * <b>After this method is called, the sensors require ~180ms to complete a measurement before data
 * is valid.</b> This means you must ensure at least this long has passed from beginning a measurement
 * until you read data in with the {@link ds18b20ReadIn()} function.
 *
 * We ensure this timing because our main program loop runs at 200ms intervals, so we begin a measurement
 * on one main loop, and then read it in on the next - giving us a ~2.5Hz read rate on temps vs 5Hz on voltage/current.
 * This is handled by {@link processTemperatures()}.
 *
 * Since our temperature sensors are used in parasitic power mode, we have to enable the strong
 * pull-up MOSFETs on the data lines to provide power while they complete a measurement.
 *
 * @see ds18b20ReadIn()
 * @see processTemperatures()
 */
void ds18b20StartMeasure()
{
	//Initialize a conversion on each bus, staggered to prevent noise on the lines
	owReset(TEMP_1);
	owWriteByte(TEMP_1, SKIP_ROM);
	owWriteByte(TEMP_1, CONVERT_T);
	pinMode(TEMP_1, INPUT);										//Set pin to high-z input to avoid burning out w/ strong pull-up
	delayMicros(7);													//Delay to ensure pin isn't output while MOSFET is conductive
	//BANK1_LOW;														//Turn on strong pull-up

	owReset(TEMP_2);
	owWriteByte(TEMP_2, SKIP_ROM);
	owWriteByte(TEMP_2, CONVERT_T);
	pinMode(TEMP_2, INPUT);	
	delayMicros(7);
	//BANK2_LOW;

}

/**
 * Reads in a temperature measurement from the three banks of DS18B20 sensors to the {@link temps} array.
 *
 * As noted in the documentation for {@link ds18b20StartMeasure()}, to have valid data at least 180ms must
 * have passed between the beginning of a measurement and this function being called to read it in.
 *
 * @see temp_t
 * @see ds18b20StartMeasure()
 */
 
 /**
 nope, that was dumb for our application. Rewrote this one, ditched their temp data structure. we only have 2 sensors, that's overkill. -sansone
 */
void ds18b20ReadIn()
{
	int16_t tempRaw = 0x0000;
	float temp = 0;

	//delayMillis(TCONV_MS);											//Wait until conversion is done
	//BANK1_HIGH;															//Turn off strong pullups to allow communication
	//BANK2_HIGH;
	//BANK3_HIGH;

	//volatile uint8_t numSkipped[3] = {0, 0, 0};

	//Read Values
	for(int8_t bank =2; bank >= 1; bank--)
	{
		for(int8_t cell = NUM_TEMPS[bank] - 1; cell >= 1; cell--)
		{
			owReset(bank);												//Reset the bus
			owWriteByte(bank, MATCH_ROM);								//Tell them we're selecting one specific device
			owWriteByte(bank,  TempAddr[bank]         & 0xFF);	//Write it's ROM address
			delayMicros(300);											//Add some delays to give the internal caps time to charge just in case we hit a long string of zeroes
			owWriteByte(bank, (TempAddr[bank]  >> 8)  & 0xFF);
			delayMicros(300);
			owWriteByte(bank, (TempAddr[bank]  >> 16) & 0xFF);
			delayMicros(300);
			owWriteByte(bank, (TempAddr[bank]  >> 24) & 0xFF);
			delayMicros(300);
			owWriteByte(bank, (TempAddr[bank]  >> 32) & 0xFF);
			delayMicros(300);
			owWriteByte(bank, (TempAddr[bank]  >> 40) & 0xFF);
			delayMicros(300);
			owWriteByte(bank, (TempAddr[bank]  >> 48) & 0xFF);
			delayMicros(300);
			owWriteByte(bank, (TempAddr[bank]  >> 56) & 0xFF);
			delayMicros(300);

			owWriteByte(bank, READ_SCRATCHPAD);							//Read in the 2-byte temp register from the scratchpad

			uint16_t lsb = 0x0000 | owReadByte(bank);
			uint16_t msb = 0x0000 | owReadByte(bank);

			tempRaw = (msb << 8) | lsb;

			owReset(bank);												//We only need the temperature bits, so reset the bus

			tempValue[bank]; = tempRaw * 0.25 * 0.25;								//Per datasheet, 10bit resolution is steps of .25C
/*
			if(temp > 5 && temp < 80)
			{
				shiftAndAverageReading(temps[bank][cell].temperature, temp);
			} else {
				numSkipped[bank]++;
			}
*/
		}
/*
		//In the event we have a shorting fault cutting out a bank, or see we dropped more than 2
		//measurements as invalid, we reconfigure the bank to get back on our feet.
		if(numSkipped[bank] > 2)
		{
			//Configure all the registers on the device properly
			owReset(bank);
			owWriteByte(bank, SKIP_ROM);									//Address all devices on the bus
			owWriteByte(bank, WRITE_SCRATCHPAD);							//Tell them we'll be writing to the scratchpad
			owWriteByte(bank, ALARM_HIGH);									//Configure high temp alarm
			owWriteByte(bank, ALARM_LOW);									//Configure low temp alarm
			owWriteByte(bank, CONFIG_REG);									//Configure to 10bit resolution
		}
        */
	}
}
