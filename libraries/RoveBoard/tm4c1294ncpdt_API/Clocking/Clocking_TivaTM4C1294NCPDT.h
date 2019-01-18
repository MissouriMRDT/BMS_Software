#ifndef CLOCKING_TIVATM4C1294NCPDT_H_
#define CLOCKING_TIVATM4C1294NCPDT_H_

/* Programmer: Drue Satterfield
 * Date of creation: September 2017
 * Microcontroller used: Tiva TM4C1294NCPDT
 * Hardware components used by this file: Hardware CPU clock, and system tick timer.
 *
 * Description: This library is used to allow the user to control the main CPU clock, as well as control other functions
 * relating to clocking and timing such as delaying and seeing how many microseconds have passed since program began.
 * The default state when initialization is called is for the CPU to go as fast as it can, 120Mhz.
 *
 * Warnings:
 * The delaying and millis/micros functions all rely on the system tick timer, as do many peripheral libraries such as ethernet;
 * do not modify the system tick timer unless you absolutely have to, and make sure you know what the effects will be.
 *
 * The functions that modify the main system clock should be done at the beginning of the program, and should be relatively constant
 * thereafter; some hardware components when initialized will stupidly keep thinking the clock is the same value that it was upon its initialization.
 * If changes to the CPU clock is made, most other libraries will need to be re-initialized.
 */

#include <stdbool.h>
#include <stdint.h>

//sets up the master system clocck to run at the default cpu frequency, and begins running the
//system tick clock so it interrupts every millisecond
//note: This function must be called before most other operations can be done, generally critical enough
//to be among the first functions ran in SystemInit
void initSystemClocks();

//changes the main CPU clock frequency based on the desired new frequency, up to 120Mhz and down to 6Mhz (latter is untested)
//warning: some critical functions such as ethernet rely on the cpu clock being the same frequency as when it was first set up, so if
//you want to change the cpu clock it's best to a) only do it at the start before doing anything else or b) turn off all peripherals
//before changing, and reset them afterwards. Generally it's best to just use the default CPU freq, as it's the fastest it can go
uint32_t setCpuClockFreq(uint32_t newFrequency);

//gets the current main CPU clock frequency in hz
uint32_t getCpuClockFreq();

//registers a user function to run everytime the systemTick clock interrupts (every millisecond).
//warning: Be certain the function won't last longer than a millisecond before you try implementing this, as otherwise
//it'll hang the program.
void registerSysTickCb(void (*userFunc)(uint32_t));

//slows down or speeds up the PIOSC internal clock, to try and calibrate it manually.
//Values lower than 0x40 slow it down, greater speeds it up. 0x40 returns it to default calibration.
//void calibratePiosc(uint16_t calValue); still in testing

//delays for some amount of milliseconds, theoretically up to however many desired but be reasonable; don't delay for tens of minutes.
//For extremely long delays, it's better to use a while loop with the millis() function.
//input: milliseconds to delay
void delay(uint32_t millis);

//delays for some amount of microseconds, accurately up to about 16383 microseconds, and DOWN accurately to about 3 microseconds
//input: Microseconds to delay
void delayMicroseconds(uint32_t micros);

//returns number of milliseconds passed since program started (or since initSystemClocks was ran, anyway).
//Rolls over after about 50 days.
uint32_t millis();

//returns number of microseconds passed since program started (or since initSystemClocks was ran, anyway).
//Rolls over after about 50 days.
uint32_t micros();

#ifdef __cplusplus
extern "C" { //so that pure c files (like RoveEthernet's underside) can use these functions without compilation errors
#endif

void cdelay(uint32_t millis);
uint32_t cmillis();

#ifdef __cplusplus
}
#endif


#endif
