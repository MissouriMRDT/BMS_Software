/**
 * @file utilities.c
 *
 * System utility functions for the battery management system.
 *
 * @date Aug 18, 2015
 * @author Jesse Cureton
 */

#include "main.h"
#include "config.h"
#include "drivers/ltc6803.h"
#include "drivers/ds18b20.h"

/**
 * Maintain the running average buffers and averaging calculation.
 *
 * Used to receive a pointer to an array, then shift all values in the array to add the new value,
 * and finally average all the values in the array based on the array size of {@link AVERAGE_PERIOD}
 *
 * @param array Pointer to the first element of a float array that we want to add valueToAdd to
 * @param valueToAdd The floating point value to add to the running average buffer
 */
void shiftAndAverageReading(float* array, float valueToAdd)
{
	P6OUT |= BIT4;
	//All measure arrays are structured [D0, D1, D2, D3, D4, ..., AVG]
	//Loop from D0-Dn-1, setting the value to that to the right of it
	for(uint8_t i = 0; i < AVERAGE_PERIOD - 1; i++)
	{
		array[i] = array[i+1];
	}

	//Set Dn-1 to the new value
	array[AVERAGE_PERIOD - 1] = valueToAdd;

	//Average the n values and properly store it
	float runningSum = 0;
	for(uint8_t i = 0; i < AVERAGE_PERIOD; i++)
	{
		runningSum += array[i];
	}

	array[AVERAGE_PERIOD] = runningSum / AVERAGE_PERIOD;
	P6OUT &= ~BIT4;
}

/**
 * Calculate the total pack voltage by summing all of our {@link cells}.
 *
 * The total pack voltage is also available as a direct reading in {@link analog_readings}[{@link ADC_PACK_VOLTAGE}][AVERAGE_PERIOD].
 *
 * @return The sum of all pack voltages from the LTC6803s and {@link cells} array.
 */
float getSummedPackVoltage()
{
	float sum = 0;
	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
	{
		for(int8_t cell = NUM_CELLS[bank]; cell >= CELL1; cell--)
		{
			sum+=cells[bank][cell].voltage[AVERAGE_PERIOD];
		}
	}
	return sum;
}

/**
 * Get the average of all cell temperatures.
 * @return The average of all cell temperatures.
 */
float getPackTemp()
{
	float sum = 0;
	uint8_t count = 0;
	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
	{
		for(int temp = NUM_TEMPS[bank] - 1; temp >= 0; temp--)
		{
			sum+=temps[bank][temp].temperature[AVERAGE_PERIOD];
			count++;
		}
	}
	return sum/count;
}

/**
 * Get the lowest-voltage cell in the {@link cells} array.
 * @return A pointer to the lowest-voltage {@link cell_t} in the {@link cells} array.
 */
cell_t* getMinCell()
{
	cell_t* lowest = &cells[NUM_BANKS][NUM_CELLS[NUM_BANKS]];

	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
	{
		for(int8_t cell = NUM_CELLS[bank]; cell >= CELL1; cell--)
		{
			if(cells[bank][cell].voltage[AVERAGE_PERIOD] < lowest->voltage[AVERAGE_PERIOD])
				lowest = &cells[bank][cell];
		}
	}
	return lowest;
}

/**
 * Get the highest-voltage cell in the {@link cells} array.
 * @return A pointer to the highest-voltage {@link cell_t} in the {@link cells} array.
 */
cell_t* getMaxCell()
{
	cell_t* highest = &cells[NUM_BANKS][NUM_CELLS[NUM_BANKS]];

	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
		{
			for(int8_t cell = NUM_CELLS[bank]; cell >= CELL1; cell--)
			{
				if(cells[bank][cell].voltage[AVERAGE_PERIOD] > highest->voltage[AVERAGE_PERIOD])
					highest = &cells[bank][cell];
			}
		}
	return highest;
}

/**
 * Get the lowest-temperature temp sensor in the {@link temps} array.
 * @return A pointer to the lowest-temperature {@link temp_t} in the {@link temps} array.
 */
temp_t* getMinTemp()
{
	temp_t* lowest = &temps[NUM_BANKS][NUM_TEMPS[NUM_BANKS] - 1];

	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
		{
			for(int8_t temp = NUM_TEMPS[bank] - 1; temp >= 0; temp--)
			{
				if(temps[bank][temp].temperature[AVERAGE_PERIOD] < lowest->temperature[AVERAGE_PERIOD])
					lowest = &temps[bank][temp];
			}
		}
	return lowest;
}

/**
 * Get the highest-temperature temp sensor in the {@link temps} array.
 * @return A pointer to the highest-temperature {@link temp_t} in the {@link temps} array.
 */
temp_t* getMaxTemp()
{
	temp_t* highest = &temps[NUM_BANKS][NUM_TEMPS[NUM_BANKS] - 1];

	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
		{
			for(int8_t temp = NUM_TEMPS[bank] - 1; temp >= 0; temp--)
			{
				if(temps[bank][temp].temperature[AVERAGE_PERIOD] > highest->temperature[AVERAGE_PERIOD])
					highest = &temps[bank][temp];
			}
		}
	return highest;
}

/*
cell_t * getMaxCellResistance()
{
	cell_t * highest = &cells[NUM_BANKS][NUM_CELLS[NUM_BANKS]]; //Assume our most positive cell
	cell_t baseline[NUM_BANKS + 1][12];

	//Internal resistance: Rinternal = Rload(Vbaseline/Vload)-Rload
	// ^^^ Is this bullshit? I think it logics out.
	float resistances[NUM_BANKS + 1][12];

	//Take a baseline reading
	ltc6803Conv();
	ltc6803ReadIn();

	//Copy the baseline reading into a temporary array
	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
	{
		for(int8_t cell = NUM_CELLS[bank]; cell >= CELL1; cell--)
		{
			baseline[bank][cell] = cells[bank][cell]; //Does this need to be a reference? I don't think so.
			// Should be a copy by value, but I'm not sure if that's defined for a typedef
		}
	}

	//this is where you enable current draw
	//this is where you read the voltage with STCVDC
	ltc6803ReadIn();		//fill ADC register with new values

	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)
	{
		for(int8_t cell = NUM_CELLS[bank]; cell >= CELL1; cell--)
		{
			float deltav = baseline[bank][cell].voltage - cells[bank][cell].voltage;

			voltage_drop = high_volt[bank][cell] - cells[bank][cell].voltage;	//calculate difference between baseline voltage and voltage while discharging
			resistance = voltage_drop / CURRENT_DRAW;		//this line for fancy resistance calculation
			if(resistance > max_resistance)
			{
				max_resistance = resistance;		//this section is to make sure no resistance exceeds a max safe resistance value.
				resistive_bank = (float)bank;		//depending on what functionality is needed, this can also just sum all resistances
				resistive_cell = (float)cell;		//to find total pack resistance.
			}
		}
	}

	//take another reading

	return highest;
}*/
