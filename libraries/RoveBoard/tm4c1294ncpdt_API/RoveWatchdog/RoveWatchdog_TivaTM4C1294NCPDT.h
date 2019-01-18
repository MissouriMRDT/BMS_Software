/*
 * RoveWatchdog.h
 *
 *  Created on: Jun 14, 2018
 *      Author: drue
 */

#ifndef ROVEBOARD_TM4C1294NCPDT_API_ROVEWATCHDOG_ROVEWATCHDOG_TIVATM4C1294NCPDT_H_
#define ROVEBOARD_TM4C1294NCPDT_API_ROVEWATCHDOG_ROVEWATCHDOG_TIVATM4C1294NCPDT_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../standardized_API/RoveWatchdog.h"

const uint8_t Watchdog0 = 0;
const uint8_t Watchdog1 = 1;

//Initializes the watchdog module for usage where it'll cause an interrupt at the specified timeout and run the
//attached function when it does
//input: watchdogId: the index of the module, Watchdog0 or Watchdog1
//       timerTimeout_us: The timeout period in microseconds. Min of 0 (which will likely just hang the program forever so
//                        why would you even), max is 35791394 us on Watchdog0, 268435455 us on Watchdog1
//       interruptFunc: the function to run when the watchdog interrupts
//returns: a handle for the now initialized watchdog module
RoveWatchdog_Handle roveWatchdog_init(int watchdogId, uint32_t timerTimeout_us, void (*interruptFunc)(void));

//Initializes the watchdog module for usage where it'll reset the program after timing out
//input: watchdogId: the index of the module, Watchdog0 or Watchdog1
//       timerTimeout_us: The timeout period in microseconds. Min of 0 (which will likely just hang the program forever so
//                        why would you even), max is 35791394 us on Watchdog0, 268435455 us on Watchdog1
//returns: a handle for the now initialized watchdog module
RoveWatchdog_Handle roveWatchdog_initReset(int watchdogId, uint32_t timerTimeout_us);

//Restarts the watchdog's countdown. Call this periodically to push the watchdog's timeout away, if there's parts of the
//program where you don't want it to trigger
//input: handle: the initialized watchdog handle who's timer you want to restart
void roveWatchdog_restartCountdown(RoveWatchdog_Handle handle);


//Stop the watchdog
//input: handle: the initialized watchdog handle who you want to stop
//Warning: For some silly reason, on the tiva you can never actually truly turn off a watchdog timer after you start it.
//         Beneath the hood roveboard simply disables its ability to reset the program and makes it so that the user's
//         interrupt function isn't run when the interrupt happens and it immediately clears, but the interrupt will still
//         occur in general even if it's no longer doing anything.
void roveWatchdog_stop(RoveWatchdog_Handle handle);

#endif /* ROVEBOARD_TM4C1294NCPDT_API_ROVEWATCHDOG_ROVEWATCHDOG_TIVATM4C1294NCPDT_H_ */
