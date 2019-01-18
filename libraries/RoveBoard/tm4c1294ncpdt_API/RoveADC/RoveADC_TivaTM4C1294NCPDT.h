/*
 * RoveADC_TivaTM4C1294NCPDT.h
 *
 * Created on: Oct 27, 2017
 *      Author: Joseph Hall, Drue Satterfield
 *
 * Microcontrollers used: tiva tm4c1294ncpdt
 * Hardware components used: The internal ADC modules, sourced by the main system clock.
 *
 * Description: This library accesses the microcontroller's ADC capabilities. The tiva uses 12 bit ADC modules, which translates to
 *  4095 degrees of resolution in its reading. Meaning that from 0 to 3.3V (the input of what the tiva takes so the max the adc can read),
 *  it has a resolution of 3.3/4095 = .805 millivolts.
 *
 *  The hardware modules on the tiva do adc in an interesting way; internally it has 8 'sequencers' that are capable of reading analog
 *  voltages from multiple pins at the same time. So if you do an adc reading on one pin using the first sequencer, it also takes the reading
 *  on any other pins assigned to that sequencer at the same time. You can save some clock cycles if you want by assigning multiple pins
 *  to a sequencer; when you start the conversion on one pin in a sequencer, you can actually then read back the results from any and all of the
 *  pins assigned to that sequencer.
 *
 *  The library takes advantage of this capability by making it so that when you call the 'start conversion' function for one sequencer and
 *  you immediately ask it to convert again with a different pin, the function will return immediately rather than spending time doing a
 *  conversion again. It will do this for further start conversion calls until a certain amount of time passes; this deadband is capable
 *  of being set by the user, in microseconds. Default is 500 microseconds
 *
 *  There are other features the hardware is capable of, such as setting how long the hardware takes to do its conversion (longer times =
 *  more resistance can be on the adc line with the reading still being fine), being triggered to convert periodically based on a timer or
 *  other sources rather than waiting to be explicitely asked before converting, and generating interrupts when conversions are complete.
 *  These may or may not be implemented, check functions below.
 *
 * Default settings:
 *  -System clock used as clocking source. Doesn't affect anything other than what the samples per second is
 *  -Samples per second is about 1.143 million samples/second
 *  -ADC module will wait until the programmer tells it to do a reading before it does.
 *  -Maximum resistance allowed between the analog source and the pin is 3500 ohms. Check this off with the hardware people.
 *
 * Pins used:
 *  There are 20 pins on the tiva that can be used to get an adc signal; these pins will work with any adc module source.
 *  The pins are listed below.
 *
 *      PE_3, PE_2, PE_1, PE_0, PD_7, PD_6, PD_5, PD_4, PE_5, PE_4, PB_4, PB_5, PD_3, PD_2, PD_1, PD_0, PK_0, PK_1, PK_2, PK_3
 *
 *
 */

#ifndef ROVEBOARD_TM4C1294NCPDT_API_ROVEADC_ROVEADC_TIVATM4C1294NCPDT_H_
#define ROVEBOARD_TM4C1294NCPDT_API_ROVEADC_ROVEADC_TIVATM4C1294NCPDT_H_

#include "standardized_API/RoveADC.h"
#include <stdbool.h>
#include <stdint.h>

//internal ADC hardware modules. There are 2 adc devices internally, and each adc device has 4 sequencers for 8 separate sequencers in total.
//Read above to learn more about the sequencers, and some benefits you get for putting pins into different channels
//on the same sequencer.
//Naming goes [adc device]_[sequencer]_[sequencer channel]
const uint8_t Adc0_Seq0_0 = 0;
const uint8_t Adc0_Seq0_1 = 1;
const uint8_t Adc0_Seq0_2 = 2;
const uint8_t Adc0_Seq0_3 = 3;
const uint8_t Adc0_Seq0_4 = 4;
const uint8_t Adc0_Seq0_5 = 5;
const uint8_t Adc0_Seq0_6 = 6;
const uint8_t Adc0_Seq0_7 = 7;
const uint8_t Adc0_Seq1_0 = 8;
const uint8_t Adc0_Seq1_1 = 9;
const uint8_t Adc0_Seq1_2 = 10;
const uint8_t Adc0_Seq1_3 = 11;
const uint8_t Adc0_Seq2_0 = 12;
const uint8_t Adc0_Seq2_1 = 13;
const uint8_t Adc0_Seq2_2 = 14;
const uint8_t Adc0_Seq2_3 = 15;
const uint8_t Adc0_Seq3_0 = 16;

const uint8_t Adc1_Seq0_0 = 17;
const uint8_t Adc1_Seq0_1 = 18;
const uint8_t Adc1_Seq0_2 = 19;
const uint8_t Adc1_Seq0_3 = 20;
const uint8_t Adc1_Seq0_4 = 21;
const uint8_t Adc1_Seq0_5 = 22;
const uint8_t Adc1_Seq0_6 = 23;
const uint8_t Adc1_Seq0_7 = 24;
const uint8_t Adc1_Seq1_0 = 25;
const uint8_t Adc1_Seq1_1 = 26;
const uint8_t Adc1_Seq1_2 = 27;
const uint8_t Adc1_Seq1_3 = 28;
const uint8_t Adc1_Seq2_0 = 29;
const uint8_t Adc1_Seq2_1 = 30;
const uint8_t Adc1_Seq2_2 = 31;
const uint8_t Adc1_Seq2_3 = 32;
const uint8_t Adc1_Seq3_0 = 33;

//Overview: initialize the adc module to read analog voltages off of a certain GPIO pin with the default settings.
//          See the constants above for what inputs can be used for modules, and the header comments above for what pin.
//          See the header comments above for what the default settings are.
//
//returns:  An initialized adc instance
//Note:     There's no error or anything if you try to have multiple modules use the same pin. It's just kind of pointless most of the time.
//Warning:  Function will go into an infinite debug fault routine if the module or the pin aren't valid, unless rove debug is disabled.
RoveAdc_Handle roveAdc_init(uint16_t module, uint8_t pin);

//Overview: Begins the process of reading an analog voltage with the given adc instance.
//Inputs:   An initialized adc instance, given from roveAdc_init
//returns:  ROVEADC_SUCCESS
RoveAdc_Error roveAdc_startConversion(RoveAdc_Handle handle);

//Overview: Gets the raw data found after a conversion has been completed with the given adc instance.
//Inputs:   An initialized adc instance, given from roveAdc_init, and a return-by-pointer variable that will contain the conversion data on return.
//          retBuff values will be between 0 and 4095, representing 0 volts to VIN volts (almost certainly 3.3V)
//returns:  ROVEADC_SUCCESS if data was successfully gotten, or ROVEADC_INCOMPLETE_CONVERSION if either the conversion is still in progress or
//          if you never called startConversion before this in the first place.
RoveAdc_Error roveAdc_getConvResults(RoveAdc_Handle handle, uint32_t *retBuff);

//Converts raw data returned from getConvResults into volts, represented as a float.
float roveAdc_toVolts(uint32_t result);

//Sets how long the program will wait between startConversion calls to actually do a conversion if two or more pins attached to the same
//sequencer are used as arguments. If startConversion is called for two or more pins in the same sequencer within this deadband, it'll simply
//return immediately and getConvResults will return the results from the last conversion.
void setSequenceConversionDeadband(uint32_t deadband_us);

#endif /* ROVEBOARD_TM4C1294NCPDT_API_ROVEADC_ROVEADC_TIVATM4C1294NCPDT_H_ */
