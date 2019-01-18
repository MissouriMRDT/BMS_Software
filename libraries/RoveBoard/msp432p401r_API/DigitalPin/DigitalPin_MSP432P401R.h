#ifndef DIGITALPIN_MSP432P401R_H_
#define DIGITALPIN_MSP432P401R_H_

/* Programmer: Drue Satterfield
 * Date of creation: October 2017
 * Microcontroller used: MSP432P401R
 * Hardware components used by this file: Potentially all GPIO pins. Pins are explicitely called into usage by the user
 * and when not initialized can be used by other hardware components.
 *
 * Description: This library is used to input and output digital signals on GPIO pins (digital signals = just a high voltage
 * representing bit 1, or a low voltage for bit 0). Voltage is in CMOS for the msp432, IE 3.3V for 1 and 0V for 0.
 * As well, the user can choose to modify the pin's setting when reading or writing signals; the available settings are
 * reading in normal mode, or with internal pull up or down resistors
 *
 * This library uses the roveboard pin mapping standard, for passing pins to the functions. Refer to the roveboard github's wiki for more info.
 *
 * Accepted pins: Refer to the pinmap file for all of them, but basically any gpio pin that's not already allocated for a special purpose
 * like crystal inputs or power inputs
 *
 * Warnings: The functions will override each other's settings if called seperately; if a special setting is used, it must be
 * specified on each call.
 */

#include <stdint.h>
#include <stdbool.h>
#include "standardized_API/DigitalPin.h"
#include "../RovePinMap_MSP432P401R.h"

//pin mode constants
const uint8_t Output = 0; //generic digital out signal (pulls signal to high or low voltage). Default output mode
const uint8_t PullUpInput = 1; //input mode with an internal pull up resistor (pulls line up if read signal is floating)
const uint8_t PullDownInput = 2; //input mode with an internal pull down resistor (pulls line down if read signal is floating)
const uint8_t Input = 3; //generic digital signal read (reads a high or low voltage). Default input mode

/*************************standard******************/

//returns if the voltage on the passed pin is at the digital HIGH voltage level or the digital LOW voltage level.
//Inputs: number of the pin to read from, as defined in the PinMap file.
//Note: This function puts the read pin into the default input mode
//returns: 1 or 0
bool digitalPinRead(uint8_t pinNumber);

//places pin into a certain input mode (or keeps it at that mode if called with same argument before),
//returns if the voltage on the passed pin is at the digital HIGH voltage level or the digital LOW voltage level.
//Inputs: number of the pin to read from, as defined in the PinMap file., and input mode, based on the above constants
//returns: 1 if high voltage, 0 if low voltage
bool digitalPinReadMode(uint8_t pinNumber, uint8_t inputMode);

//writes a High voltage (equal to data bit 1) or a Low voltage (equal to data bit 0) on the passed pin.
//Inputs: number of the pin to read from, as defined in the PinMap file. 1 for high output, 0 for low output.
//Note: This function puts the write pint into the default output mode
//returns true if successful, false if incorrect arguments are passed.
bool digitalPinWrite(uint8_t pinNumber, bool outputLevel);

//places pin into a certain output mode. Note that on the msp432, the only output mode is Output anyway so kept here
//  purely for standardized purposes.
//  writes a High voltage (equal to data bit 1) or a Low voltage (equal to data bit 0) on the passed pin.
//Inputs: number of the pin to read from, as defined in the board's PinMap file. 1 for high output, 0 for low output.
//		  Output mode, based on the constants above
//returns true if successful, false if incorrect arguments are passed.
bool digitalPinWriteMode(uint8_t pinNumber, bool outputLevel, uint8_t outputMode);

#endif
