#ifndef TIMERINTERFACE_TIVATM4C1294NCPDT_H_
#define TIMERINTERFACE_TIVATM4C1294NCPDT_H_

/* Programmer:  Drue Satterfield
 * Date of creation: September 2017.
 * Microcontroller used: Tiva TM4C1294NCPDT
 * Hardware components used by this file: all timers can possibly be used, from 0 to 7; timers are explicitely called for operation by user
 *    and when not initialized can be used for other projects instead.
 *
 * Update 10/4/2017: Redid external API so that it now uses a timer handle, to enforce the user actually calling init
 *
 *
 * Description: This library is used to implement basic timer capabilities relating to timing out and periodic interrupting.
 */

#include <stdint.h>
#include <stdbool.h>
#include "../../standardized_API/RoveTimer.h"

//timer id's
const uint8_t Timer0 = 0;
const uint8_t Timer1 = 1;
const uint8_t Timer2 = 2;
const uint8_t Timer3 = 3;
const uint8_t Timer4 = 4;
const uint8_t Timer5 = 5;
const uint8_t Timer6 = 6;
const uint8_t Timer7 = 7;

//interrupt id's
#define TimerPeriodicInterrupt 0

//sets up the specified timer to generate the specified interrupt at a specified rate
//Input: Timer Id, the interruptId based on above constants, how frequently
//the timer runs the interrupt in microseconds, and the function to run when the timer times out.
//Min is 1 us, max is about 268 seconds
//Warning: Function enters a fault infinite loop if arguments are incorrect (if timerId or interruptId aren't one of the above consts)
RoveTimer_Handle setupTimer(uint32_t timerId, uint32_t interruptId, uint32_t timerTimeout_us, void (*interruptFunc)(void));

//begins timer operation
//inputs: handle of the timer to start
void startTimer(RoveTimer_Handle handle);

//stops timer operation
//inputs: handle of the timer to stop
void stopTimer(RoveTimer_Handle handle);

#endif
