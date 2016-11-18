/**
 * @file adc.h
 *
 * @author Jesse Cureton
 * @date 5 March 2016
 */

#ifndef ADC_H_
#define ADC_H_

#include "config.h"

#define VOLTS_PER_SAMPLE	(float)  0.0008057	///< The number of volts per sample count for the ADC12 peripheral, Vcc/resolution
#define PACK_VOLTAGE_SCALE 	(float) 41.0000000	///< Pack voltage divider divides by 41 per the schematic
#define PACK_CURRENT_VOFST  (float)  2.5000000	///< Current divider voltage readings are offset 2.5V from 0, since range of sensor is +/- 100A and output is 0-5V
#define PACK_CURRENT_VSCALE (float)  1.6666667	///< Voltage divider for current sensor input divides by 5/3
#define PACK_CURRENT_ISCALE	(float)  80.000000  ///< Per the sensor datasheet this is the scale of the sensor, 80 Amps/Volt
#define AUX_VOLTAGE_SCALE 	(float)  4.7037037 	///< Aux pack voltage divider divides by 4.703703

/**
 * The global-scope array of floats containing the readings for all things measured by the onboard ADC.
 *
 * Indexed as analog_readings[{@link ADC_INDEXES}][count] where count is which sample. As with all other data readings on the board,
 * {@link AVERAGE_PERIOD} samples are stored and averaged, and the average of the last <i>n</i> samples is accessible at index AVERAGE_PERIOD.
 */
float analog_readings[3][AVERAGE_PERIOD + 1];

/**
 * The global-scope floating point value of the maximum remaining current capacity of the pack.
 *
 * Used tell the DE how much current it can pump in via regen, and is found with net pack current - motor current
 * to calculate the array current, which is then subtracted from the max current
 */
float allowableInputCurrent;

void adc_init();
void measure_analogs();

#endif /* ADC_H_ */
