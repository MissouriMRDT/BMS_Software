/**
 * @file adc.c
 *
 * @author Jesse Cureton
 * @date 5 March 2016
 */

#include "datatypes.h"
#include "adc.h"
#include "main.h"
#include <msp430.h>

/**
 * Initialize the onboard ADC12 peripheral on all used channels.
 *
 * We use ADC12 channels 0, 1, and 2 to measure pack voltage, pack current, and aux pack voltage, respectively.
 * We initialize the peripheral to take a single sequence sample of all three channels, and only when we manually start a conversion.
 * All measurements are referenced against AVcc and AVss, which are just VCC and GND on the chip since DVcc and DVss are connected internally.
 */
void adc_init()
{
	ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_8; 		// Turn on ADC12, set sampling time
	ADC12CTL1 = ADC12SHP+ADC12CONSEQ_1;       		// Use sampling timer, single sequence
	ADC12MCTL0 = ADC12INCH_0;                		// ref+=AVcc, channel = A0
	ADC12MCTL1 = ADC12INCH_1;                		// ref+=AVcc, channel = A1
	ADC12MCTL2 = ADC12INCH_2+ADC12EOS;        	    // ref+=AVcc, channel = A2, end seq.
}

/**
 * Begin a measurement of pack voltage, pack current, and aux pack voltage.
 *
 * Starts a one-off conversion sequence on the ADC12 peripheral to read in the required data, average it, and add it to the {@link analog_readings} array.
 */
void measure_analogs()
{
	ADC12CTL0 |= ADC12ENC;                    		// Enable conversions
	ADC12CTL0 |= ADC12SC;							// Start conversion

	 //Wait for conversion to finish
	while((ADC12IFG & (ADC12IFG0 + ADC12IFG1 + ADC12IFG2)) != (ADC12IFG0 + ADC12IFG1 + ADC12IFG2));

	shiftAndAverageReading(analog_readings[ADC_PACK_VOLTAGE], (ADC12MEM0 * VOLTS_PER_SAMPLE * PACK_VOLTAGE_SCALE));
	shiftAndAverageReading(analog_readings[ADC_PACK_CURRENT], (((ADC12MEM1 * VOLTS_PER_SAMPLE * PACK_CURRENT_VSCALE) - PACK_CURRENT_VOFST) * PACK_CURRENT_ISCALE));
	shiftAndAverageReading(analog_readings[ADC_AUX_VOLTAGE], (ADC12MEM2 * VOLTS_PER_SAMPLE * AUX_VOLTAGE_SCALE));

	//analog_readings[ADC_PACK_VOLTAGE][0]  = ADC12MEM0 * VOLTS_PER_SAMPLE;	//Convert to voltage at the ADC pin
	//analog_readings[ADC_PACK_VOLTAGE][0] *= PACK_VOLTAGE_SCALE;			//Convert to voltage at start of pack divider

	//analog_readings[ADC_PACK_CURRENT][0]  = ADC12MEM1 * VOLTS_PER_SAMPLE;	//Convert to voltage at ADC input
	//analog_readings[ADC_PACK_CURRENT][0] *= PACK_CURRENT_VSCALE;			//Convert to voltage before divider (at sensor output)
	//analog_readings[ADC_PACK_CURRENT][0] -= PACK_CURRENT_VOFST;			//Handle the 2.5V offset
	//analog_readings[ADC_PACK_CURRENT][0] *= PACK_CURRENT_ISCALE;			//Scale the -2.5 to 2.5V reading to signed current

	//analog_readings[ADC_AUX_VOLTAGE][0]   = ADC12MEM2 * VOLTS_PER_SAMPLE;
	//analog_readings[ADC_AUX_VOLTAGE][0]  *= AUX_VOLTAGE_SCALE;
}
