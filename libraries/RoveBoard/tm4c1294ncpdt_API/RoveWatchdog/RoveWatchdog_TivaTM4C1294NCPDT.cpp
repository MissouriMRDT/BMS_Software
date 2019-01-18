/*
 * RoveWatchdog_TivaTM4C1294NCPDT.cpp
 *
 *  Created on: Jun 14, 2018
 *      Author: drue
 */

#include "RoveWatchdog_TivaTM4C1294NCPDT.h"
#include "../Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "../../supportingUtilities/Debug.h"
#include "../tivaware/inc/hw_ints.h"
#include "../tivaware/driverlib/interrupt.h"
#include "../tivaware/driverlib/sysctl.h"
#include "../tivaware/inc/hw_memmap.h"
#include "../tivaware/driverlib/watchdog.h"

const float PIOSCClockFreq = 16000000; //frequency of the internal precision clock, which watchdog 1 uses

typedef struct WatchdogData
{
  uint8_t watchdogId; //Id by which the functions track watchdog modules, constants defined in the .h file
  uint32_t watchdogBase; //the hardware memory location of the watchdog module, for firmware calls
  uint32_t watchdogPeriphId; //the Id of the watchdog module for use in firmware calls that require a peripheral id
  void (*attachedFunction)(void); //the function the user has asked to be ran when the watchdog interrupts
  void (*baseFunction)(void); //the base function, contained in this file, that the watchdog calls when interrupting

  WatchdogData(uint8_t tId, uint32_t tB, uint32_t tPI, void (*intHandler)(void))
  {
    watchdogId = tId;
    watchdogBase = tB;
    watchdogPeriphId = tPI;
    baseFunction = intHandler;
    attachedFunction = 0;
  }
} WatchdogData;

static void watchdog0Handler();
static void watchdog1Handler();
static WatchdogData* commonInit(int watchdogId, uint32_t timerTimeout_us, bool reset);
static WatchdogData* getWatchdogData(int watchdogId);

static WatchdogData watchdog0Data(Watchdog0, WATCHDOG0_BASE, SYSCTL_PERIPH_WDOG0, watchdog0Handler);
static WatchdogData watchdog1Data(Watchdog1, WATCHDOG1_BASE, SYSCTL_PERIPH_WDOG1, watchdog1Handler);

RoveWatchdog_Handle roveWatchdog_init(int watchdogId, uint32_t timerTimeout_us, void (*interruptFunc)(void))
{
  WatchdogData* data = commonInit(watchdogId, timerTimeout_us, false);
  data->attachedFunction = interruptFunc;

  RoveWatchdog_Handle handle;
  handle.index = watchdogId;
  handle.initialized = true;

  return handle;
}

RoveWatchdog_Handle roveWatchdog_initReset(int watchdogId, uint32_t timerTimeout_us)
{
  commonInit(watchdogId, timerTimeout_us, true);

  RoveWatchdog_Handle handle;
  handle.index = watchdogId;
  handle.initialized = true;

  return handle;
}

void roveWatchdog_restartCountdown(RoveWatchdog_Handle handle)
{
  WatchdogData *data = getWatchdogData(handle.index);
  if(data == 0 || handle.initialized == false)
  {
    debugFault("roveWatchdog_restart: handle not initted or otherwise has improper data");
  }

  if(WatchdogRunning(data->watchdogBase))
  {
    //calling the interrupt clear also causes the timer to
    //be reset, coincidentally, so we can restart it just by
    //clearing the flag
    WatchdogIntClear(data->watchdogBase);
  }
}

void roveWatchdog_stop(RoveWatchdog_Handle handle)
{
  WatchdogData *data = getWatchdogData(handle.index);
  if(data == 0 || handle.initialized == false)
  {
    debugFault("roveWatchdog_stop: handle not initted or otherwise has improper data");
  }

  //turn off watchdog's ability to reset and also
  //make it so nothing happens when it interrupts.
  //We don't just turn off the interrupt period because
  // ...................
  // ...cause we actually can't, the tiva won't stop it
  // after it's been turned on. Go figure.
  WatchdogResetDisable(data->watchdogBase);
  data->attachedFunction = 0;
}

static void watchdog0Handler()
{
  WatchdogIntClear(watchdog0Data.watchdogBase);

  if(watchdog0Data.attachedFunction != 0)
  {
    watchdog0Data.attachedFunction();
  }
  else
  {
    //very slightly delay so the int clears properly
    SysCtlDelay(5);
  }
}

static void watchdog1Handler()
{
  WatchdogIntClear(watchdog1Data.watchdogBase);

  if(watchdog1Data.attachedFunction != 0)
  {
    watchdog1Data.attachedFunction();
  }
  else
  {
    //very slightly delay so the int clears properly
    SysCtlDelay(5);
  }
}

static WatchdogData* commonInit(int watchdogId, uint32_t timerTimeout_us, bool reset)
{
  WatchdogData *data;
  uint64_t clockFreq;

  if(watchdogId == 0)
  {
    data = &watchdog0Data;
    clockFreq = getCpuClockFreq();
  }
  else if(watchdogId == 1)
  {
    data = &watchdog1Data;
    clockFreq = PIOSCClockFreq;
  }
  else
  {
    debugFault("roveWatchdog_commonInit: watchdogId is garbage and crap and out of bounds (should be 0 or 1)");
  }

  uint32_t load = clockFreq * (timerTimeout_us/1000000.0); // clock cycle (120 MHz cycle/second) * (microsecond timeout/10000000 to convert it to seconds) = cycles till the timeout passes

  if(reset)
  {
    load /=2; //watchdog resets after two timeouts
  }

  //
  // Enable the Watchdog peripheral
  //
  SysCtlPeripheralEnable(data->watchdogPeriphId);

  //
  // Wait for the Watchdog module to be ready.
  //
  while(!SysCtlPeripheralReady(data->watchdogPeriphId));

  //
  // Initialize the watchdog timer.
  //
  WatchdogReloadSet(data->watchdogBase, load);

  //
  // Prevents watchdog from firing during debugging
  //
  WatchdogStallEnable(data->watchdogBase);

  if(reset)
  {
    // Enable the reset
    WatchdogResetEnable(data->watchdogBase);
  }
  else
  {
    //enable watchdog interrupts
    WatchdogIntRegister(data->watchdogBase, data->baseFunction);
    WatchdogIntEnable(data->watchdogBase);
    IntEnable(INT_WATCHDOG);
  }

  //
  // Enable the watchdog timer.
  //
  WatchdogEnable(data->watchdogBase);

  return data;
}

static WatchdogData* getWatchdogData(int watchdogId)
{
  WatchdogData *data;

  if(watchdogId == 0)
  {
    data = &watchdog0Data;
  }
  else if(watchdogId == 1)
  {
    data = &watchdog1Data;
  }
  else
  {
    data = 0;
  }

  return data;
}
