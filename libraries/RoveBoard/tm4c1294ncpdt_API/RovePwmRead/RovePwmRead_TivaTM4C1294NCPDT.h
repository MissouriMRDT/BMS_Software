/* Programmers: Drue Satterfield, Timur Guler
 * Date of creation: 10/14/2016
 * Microcontroller used: Tiva TM4C1294NCPDT
 * Hardware components used by this file: Internal timers 0-5
 * 
 * Update 10/17/16: under nominal PWM conditions, return values work on timer 1. Edge conditions do not work, timeout interrupt is buggy as hell, currently disabled in timer setup
 * Update 10/20/16: works for 0% and 100%. All pins tested.
 * Update 10/21/16: Works when all 5 are on at once
 * Update 11/3/16: added pin map layer so the user now only has to pass in one argument
 * Update 10/4/17: Redid external API to now need a pwmRead handle instance, to enforce the user actually calling init beforehand
 * Update 10/9/17: Added ability to use timer0 -- Timur Guler
 * 
 * Description: This library is used to read a pwm signal on 
 * pins utilized by timers 0-5. The program is started by calling the
 * pwmStart function, and afterwords the program shall use
 * the timer attached to that pin to monitor for incoming
 * pulses. When the voltage on that pin changes, the timer
 * triggers an interrupt, and the time of the voltage change
 * is recorded. By doing this, it finds the time in microseconds
 * of how long the pulse was high, and how long it was low. 
 * Thus it calculates the on period, off period, total period,
 * and duty cycle. The duty cycle, total period, and on period
 * of the most recent pulse can be read by calling the related
 * get functions. 
 *
 * This library uses the roveboard pin mapping standard, for passing pins to the functions. Refer to the roveboard github's wiki for more info.
 *
 * Acceptable pins (and which timer they use, don't try to use multiple pins that use the same timer)
 * A0(t0), A2 (t1), A4(t2), A6(t3), B0(t4), B2(t5), D0(t0), D2(t1), D4(t3), L4(t0), L6(t1), M0(t2), M2(t3), M4(t4), M6(t5)
 *
 * Warnings: The file triggers interrupts quite frequently, 
 * once ever time the voltage on the pin changes and a second 
 * timed interrupt that occurs roughly once every half a second
 * (it's there to basically monitor the line and detect
 * if the line has 0% or 100% duty). It can be processor-intensive.
 * Also the first pulse of a transmission can sometimes be 
 * disregarded as garbage data depending on conditions, 
 * though this doesn't tend to matter in pwm transmissions.
 *
 * Minimum frequency that can be read is 1 hz. Accuracy: can read difference of 62.5 nanoseconds between pulses
 */

#ifndef PWMREADER_TIVATM4C1294NCPDT_H_
#define PWMREADER_TIVATM4C1294NCPDT_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../standardized_API/RovePwmRead.h"

const int ReadModule0 = 0;
const int ReadModule1 = 1;
const int ReadModule2 = 2;
const int ReadModule3 = 3;
const int ReadModule4 = 4;
const int ReadModule5 = 5;

//interrupt priorities on the tiva go from 0-7, 0 highest priority.
//edge capture interrupt = interrupt triggered when a pwm wave comes in from the gpio pin
//timeout interrupt = interrupt triggered when timer times out (ran in parallel with edge capture, used
// to detect when no edge has come in in a certain amount of time)
const uint8_t PwmReadEdgeCaptureInterruptPriority_Default = 2;
const uint8_t PwmReadTimeoutInterruptPriority_Default = 3;

//Begins reading pwm pulses on the specified pin using the specified timer.
//Input: The pwmRead module to use, and which of its associated GPIO pins are to be used
//Returns a rovePwmRead handle with internal settings initialized
//warning: if the arguments are invalid, the function enters an infinite loop fault routine for checking in a debugger
RovePwmRead_Handle initPwmRead(uint8_t readingModule, uint8_t mappedPin);

//Stops reading pwm. 
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
void stopPwmRead(RovePwmRead_Handle handle);

//gets the duty cycle being read on the specified pin.
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
//Output: 0-100 duty cycle
uint8_t getDuty(RovePwmRead_Handle handle);

//gets the total period of the PWM signal last transmitted for 
//the specified pin
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
//Output: period of last transmission in microseconds
uint32_t getTotalPeriod(RovePwmRead_Handle handle, RovePwmRead_Scale scale);

//Gets the on period of the last tramsittted PWM signal for
//the specified pin
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
//Output: On-period of pulse in microseconds
uint32_t getOnPeriod(RovePwmRead_Handle handle, RovePwmRead_Scale scale);

//Sets the interrupt priority for the specified reading instance
//Input: The handle for the pwm reading instance to modify, the priority of the edge capture, priority of timeout.
//       The priority is between 0 and 7, with 0 being highest priority.
//Note: initPwmRead must be called before hand
void setPwmReadInterruptPriority(RovePwmRead_Handle handle, uint8_t edgeCapturePriority, uint8_t timeoutCapturePriority);

//Sets how many microseconds it takes for the disconnect check timer to trigger. The quicker it is the quicker it'll be to
//detect disconnects, but it must never be shorter than the pwm signal's period
//input: The handle of th pwm reading instance to modify
//       The microseconds of the timeout. Max is 1 second.
//warning: it must never be shorter than the pwm signal's period
void setDisconnectCheckTimeout(RovePwmRead_Handle handle, uint32_t timeout_us);

#endif
