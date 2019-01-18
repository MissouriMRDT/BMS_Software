#ifndef CLOCKING_TIVATM4C1294NCPDT_H_
#define CLOCKING_TIVATM4C1294NCPDT_H_

/* Programmer: Drue Satterfield
 * Date of creation: September 2017
 * Microcontroller used: MSP432P401R
 * Hardware components used by this file: Hardware CPU clock, and system tick timer.
 *
 * Description: This library is used to allow the user to control the main CPU clock, as well as control other functions
 * relating to clocking and timing such as delaying and seeing how many microseconds have passed since program began.
 * The default state when initialization is called is for the CPU to go as fast as it can, 48Mhz.
 *
 * Warnings:
 * The delaying and millis/micros functions all rely on the system tick timer;
 * do not modify the system tick timer unless you absolutely have to, and make sure you know what the effects will be.
 *
 * The functions that modify the main system clock should be done at the beginning of the program, and should be relatively constant
 * thereafter; some hardware components when initialized will stupidly keep thinking the clock is the same value that it was upon its initialization.
 * If changes to the CPU clock is made, most other libraries will need to be re-initialized.
 *
 * Power control:
 * The system shall automatically select the optimum power mode for the chip, depending on the clock speeds selected when changed. See Power.h
 * for more info on how the different modes affect chip power consumption.
 */

#include <stdbool.h>
#include <stdint.h>

#define EXTERNAL_LOW_FREQUENCY_CRYSTAL_FREQ 32000 //standard msp432 boards use 32khz crystals for low frequency, and 48Mhz for high frequency
#define EXTERNAL_HIGH_FREQUENCY_CRYSTAL_FREQ 48000000

// the msp432 has two voltage modes, high and min voltage, based on how fast the clocks are going. This is the highest frequency that
//the main CPU clock can be going while still in min voltage mode. Beyond this the software will change to high voltage mode.
#define MinVoltageMode_CpuMaxFreq 24000000
#define MinVoltageMode_PeriphMaxFreq 12000000

//inits the system clocks. Should already be done in sysInit, there's typically not a reason for the user to directly call this.
void initSystemClocks();

//sets the cpu clock frequency.
//inputs: Desired clock frequency for the cpu clock, in hz
//returns: The actual new clock frequency, in hz
//note: if the new clock frequency and the periph clock freq are both below their minimum voltage to be in min voltage mode (see the consts above)
//      then the system will automatically shift into minimum voltage mode. Else it'll shift into high voltage mode. Nothing really changes
//      besides how much power the chip is consuming
//Warning: When changing clock frequency, it's best to re-init any peripherals you're using so that they are notified of the change in
//clock speed
uint32_t setCpuClockFreq(uint32_t newFrequency);

//sets the periph clock frequency.
//inputs: Desired clock frequency for the periph clock, in hz
//returns: The actual new clock frequency, in hz
//note: if the new clock frequency and the cpu clock freq are both below their minimum voltage to be in min voltage mode (see the consts above)
//      then the system will automatically shift into minimum voltage mode. Else it'll shift into high voltage mode. Nothing really changes
//      besides how much power the chip is consuming
//Warning: When changing clock frequency, it's best to re-init any peripherals you're using so that they are notified of the change in
//clock speed
uint32_t setPeriphClockFreq(uint32_t newFrequency);

//gets the current cpu clock frequency in hz
uint32_t getCpuClockFreq();

//gets the current periph clock frequency in hz
uint32_t getPeriphClockFreq();

//delays for some amount of milliseconds. System still operational in the background and interrupts will keep firing.
void delay(uint32_t millis);

//delays for some amount of microseconds. System still operational in the background and interrupts will keep firing.
void delayMicroseconds(uint32_t microsToDelay);

//gets how many milliseconds have passed since the beginning of the program.
uint32_t millis();

//gets how many microseconds have passed since the beginning of the program.
uint32_t micros();

#endif
