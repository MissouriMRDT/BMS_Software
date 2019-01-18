/*
 * RoveWatchdog.h
 *
 *  Created on: Jun 14, 2018
 *      Author: drue
 */

#ifndef ROVEBOARD_STANDARDIZED_API_ROVEWATCHDOG_H_
#define ROVEBOARD_STANDARDIZED_API_ROVEWATCHDOG_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct RoveWatchdog_Handle
{
    bool initialized;
    int index;

#ifdef __cplusplus
    RoveWatchdog_Handle()
    {
      initialized = false;
      index = -1;
    }
#endif

}RoveWatchdog_Handle;

//Initializes the watchdog module for usage where it'll cause an interrupt at the specified timeout and run the
//attached function when it does
//input: watchdogId: the index of the module, based on board specific constants
//       timerTimeout_us: The timeout period in microseconds. Acceptable inputs are board specific
//       interruptFunc: the function to run when the watchdog interrupts
//returns: a handle for the now initialized watchdog module
extern RoveWatchdog_Handle roveWatchdog_init(int watchdogId, uint32_t timerTimeout_us, void (*interruptFunc)(void));

//Restarts the watchdog's countdown. Call this periodically to push the watchdog's timeout away, if there's parts of the
//program where you don't want it to trigger
//input: handle: the initialized watchdog handle who's timer you want to restart
extern void roveWatchdog_restartCountdown(RoveWatchdog_Handle handle);

//Stop the watchdog
//input: handle: the initialized watchdog handle who you want to stop
extern void roveWatchdog_stop(RoveWatchdog_Handle handle);


#endif /* ROVEBOARD_STANDARDIZED_API_ROVEWATCHDOG_H_ */
