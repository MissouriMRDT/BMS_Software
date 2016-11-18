/**
 * @file ltc6803.c
 *
 * Driver for the <a href="http://www.linear.com/product/LTC6803-1">LTC6803</a> battery stack monitor IC.
 *
 * @author Jesse Cureton
 */
#include "msp430.h"
#include "main.h"
#include "config.h"
#include "datatypes.h"
#include "pins.h"
#include "spi.h"

#include "drivers/ltc6803.h"

/**
 * Write the needed configuration to the three stacked LTC6803 ICs, with the ability to set the comparator duty cycle.
 *
 * @param cdcval Comparator duty cycle value - one of {@link LTC6803_CDC_GROUP}
 */
void ltc6803_write_config(uint8_t cdcval)
{
	LTC_SEL;														//Select the LTC banks
	spi_tx(LTC_BUS, WRCFG);											//Tell them we want to write to config registers
	spi_tx(LTC_BUS, WRCFG_PEC);										//Send the PEC packet for the config code
	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)				//Must push them down into a stack with BANK3 first
	{
		RESET_PEC;
		for(int8_t i = 0; i < 6; i++)								//Send all 6 configuration bytes
		{
			if(i == 0) spi_tx(LTC_BUS, CFGR[i] | cdcval);
			else if(bank == BANK1 && i == 3) spi_tx(LTC_BUS, CFGR[i] & ~0x30);//Bank 1 has 10 cells, not 8, so don't mask it's top two
			else spi_tx(LTC_BUS, CFGR[i]);							//Barring the special case above send the predefined config values
			if(i == 0) ltc6803PEC(CFGR[i] | cdcval);
			else if(bank == BANK1 && i == 3) ltc6803PEC(CFGR[i] & ~0x30);//Special case for the PEC for BANK1
			else ltc6803PEC(CFGR[i]);								//Update the PEC with the bit that was sent
		}
		spi_tx(LTC_BUS, pecVAL);									//Send the PEC byte for the 6byte register we just sent
	}
	LTC_DESEL;														//Release the bus and latch data into the shift register stack
}

/**
 * Begin an LTC6803 conversion.
 *
 * The conversion takes roughly 25ms, and the result lives on the LTC6803 internal conversion memory.
 */
void ltc6803Conv()
{
	ltc6803_write_config(CDC1);										//Take the chips out of standby
	LTC_SEL;														//Select the LTC banks
	spi_tx(LTC_BUS, STCVAD);										//Send a convert all command
	spi_tx(LTC_BUS, STCVAD_PEC);									//PEC byte
	delayMillis(20);												//Pg. 8 of datasheet has a Cell Conversion time table - max of 13.1ms
																	//We could poll the SDO pin to check these, but thats annoying and we can just delay
	LTC_DESEL;														//Deselect the banks
	ltc6803_write_config(CDC0);										//Back in standby
}

/**
 * Read in converted voltages from the LTC6803 internal conversion memory.
 */
void ltc6803ReadIn()
{
	uint8_t cellRegs[3][15];										//3 banks of 15 CVR registers storing data for our 10 cells
	uint8_t	readPECs[3];											//The PEC values we read in - for integrity checking
	static uint8_t contCommsFaults = 0;								//The number of continuous communications faults we have received
	static uint8_t contBoundsFaults[NUM_BANKS][12];					//The number of continuous value out of bounds errors we have received
	LTC_SEL;
	spi_tx(LTC_BUS, RDCV);
	spi_tx(LTC_BUS, RDCV_PEC);

	for(uint8_t bank = BANK1; bank <= NUM_BANKS; bank++)			//Read in the raw CVRxx registers from the LTC stack
	{
		RESET_PEC;
		for(uint8_t i = 0; i < 15; i++)
		{
			cellRegs[bank][i] = spi_tx(LTC_BUS, RDCV);
			ltc6803PEC(cellRegs[bank][i]);
		}
		readPECs[bank] = spi_tx(LTC_BUS, 0x00);
		if(readPECs[bank] != pecVAL)
		{
			contCommsFaults++;
			if(contCommsFaults == 5)
			{
				ERRORS |= COMMS_FAULT;								//Communication fault - checksums don't match
			}
		} else {
			contCommsFaults = 0;									//No error, so reset the count
		}
	}
	LTC_DESEL;

	for(int8_t bank2 = NUM_BANKS; bank2 >= BANK1; bank2--)			//Convert these values to actual voltages in the right memory array
	{
		float vtgs[10];

		//Horrible, dark magic. Abandon all hope ye who enter here.
		//This separates the 12-bit readings split in 2 8-bit registers into variables still as raw data
		vtgs[CELL1]  = (cellRegs[bank2][0]  & 0xFF) 	 | ((cellRegs[bank2][1]  & 0x0F) << 8);
		vtgs[CELL2]  = (cellRegs[bank2][1]  & 0xF0) >> 4 | ((cellRegs[bank2][2]  & 0xFF) << 4);
		vtgs[CELL3]  = (cellRegs[bank2][3]  & 0xFF)	   	 | ((cellRegs[bank2][4]  & 0x0F) << 8);
		vtgs[CELL4]  = (cellRegs[bank2][4]  & 0xF0) >> 4 | ((cellRegs[bank2][5]  & 0xFF) << 4);
		vtgs[CELL5]  = (cellRegs[bank2][6]  & 0xFF)	     | ((cellRegs[bank2][7]  & 0x0F) << 8);
		vtgs[CELL6]  = (cellRegs[bank2][7]  & 0xF0) >> 4 | ((cellRegs[bank2][8]  & 0xFF) << 4);
		vtgs[CELL7]  = (cellRegs[bank2][9]  & 0xFF)	     | ((cellRegs[bank2][10] & 0x0F) << 8);
		vtgs[CELL8]  = (cellRegs[bank2][10] & 0xF0) >> 4 | ((cellRegs[bank2][11] & 0xFF) << 4);
		vtgs[CELL9]  = (cellRegs[bank2][12] & 0xFF)	     | ((cellRegs[bank2][13] & 0x0F) << 8);
		vtgs[CELL10] = (cellRegs[bank2][13] & 0xF0) >> 4 | ((cellRegs[bank2][14] & 0xFF) << 4);

		for(int8_t cell = NUM_CELLS[bank2]; cell >= CELL1; cell--)			//Convert the raw data in the registers to actual voltage floats
		{
			cells[bank2][cell].cell = cell;
			cells[bank2][cell].bank = bank2;

			//Convert raw vtg readings to actual voltages
			vtgs[cell] -= 512;
			vtgs[cell] *= 1.5 * .001;

			//Store readings in averaged array
			if(vtgs[cell] <= 2.0 || vtgs[cell] >= 4.8)
			{
				contBoundsFaults[bank2][cell]++;
				if(contBoundsFaults[bank2][cell] == 5)
				{
					ERRORS |= OV_FAULT + UV_FAULT;
				}
			} else {
				contBoundsFaults[bank2][cell] = 0;
				shiftAndAverageReading(cells[bank2][cell].voltage, vtgs[cell]);
			}
		}
	}

	//Clear the voltage registers so we trigger a fault on the next read if there isn't data
	LTC_SEL;
	spi_tx(LTC_BUS, STCCLR);
	spi_tx(LTC_BUS, STCCLR_PEC);
	LTC_DESEL;
}

/**
 * Packet Error Correction calculation to ensure data integrity during communication.
 *
 * Implements an 8-bit cyclic redundancy check as specified on page 17 of the <a href="http://cds.linear.com/docs/en/datasheet/680313fa.pdf">datasheet</a>.
 * See <a href="http://en.wikipedia.org/wiki/Cyclic_redundancy_check">Wikipedia</a> if you're interested in this.
 *
 * Because this calculation is time-dependent as defined in the datasheet - that is, it's a running
 * calculation for the entirety of one message set, not just byte-by-byte - it must have some way to
 * track variables across function calls. These could be implemented as a <i>static</i> variable in
 * the function, but the final calculated PEC values are needed elsewhere. Thus {@link pecVAL} and {@link pecIN}
 * are implemented in {@link ltc6803.h} and available at the global scope.
 *
 * <b>The {@link RESET_PEC} macro must be called at the start of a transmission to ensure that the
 * PEC values are accurate for each transmission.</b>
 *
 * Common PEC values are stored for the basic commands in {@link LTC6803_CMD_PECS}. Any data we
 * send or receive needs to be PEC'd at tx/rx time, before we send the PEC byte at the end of our
 * TX message.
 *
 * @param val The value to calculate a PEC byte for.
 */
void ltc6803PEC(uint8_t val)
{
	uint8_t curBit = 0;												//8-bit value with the current bit in its LSB
	for(int8_t i = 0; i < 8; i++)									//Loop through every bit in the byte
	{
		curBit   = (val >> (7-i)) & 0x01;							//Isolate the the (i-1)th bit of val
		pecIN    = curBit ^ ((pecVAL >> 7) & 0x01);					//Initialize pecIN to (0b0000000X where X is curBit) XOR MSB of PEC
		pecIN   |= ((pecVAL & 0x01) ^ (pecIN & 0x01)) << 1;			//Magic 		(IN1 = PEC[0] XOR IN0)
		pecIN   |= (((pecVAL >> 1) & 0x01) ^ (pecIN & 0x01)) << 2; 	//More magic 	(IN2 = PEC[1] XOR IN0)
		pecVAL	 = ((pecVAL << 1) & ~0x07) | pecIN;					//Set the current PEC value
	}
}
