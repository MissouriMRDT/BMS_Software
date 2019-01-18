/*
 * Clocking_MSP432P401R.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: drue
 */

#include "Clocking_MSP432P401R.h"
#include "../msp432Ware/driverlib/driverlib.h"
#include "../msp432Ware/inc/msp432p401r.h"
#include "supportingUtilities/Debug.h"
#include "supportingUtilities/RoveUtilities.h"

static void SysTickIntHandler();
static bool needChangeVoltageStateFirst(uint32_t desiredCpuFreq, uint32_t desiredPeriphFreq);
static void selectOptimalVoltageState(uint32_t cpuFreq, uint32_t periphFreq);
static uint32_t setClockFreq(int64_t newFrequency, uint32_t clockToSet);

static const uint16_t SystickIntPriority = 0x80;
static uint32_t milliseconds = 0;
static uint16_t SystickHz = 1000;

static bool hfxtGood = false;
static bool lfxtGood = false;

#define HFXTCLK_MINIMUM (EXTERNAL_HIGH_FREQUENCY_CRYSTAL_FREQ / 128) //lowest clock divider is 128, so this is the minimum freq we can get on HFXT
#define DCOCLK_MINIMUM (CS_getDCOFrequency() / 128)

#define REFOCLK_FREQ (128000)

#define CpuMaxFreq 48000000
#define PeriphMaxFreq 24000000
#define DCOMaxFreq 48000000

void initSystemClocks()
{
  int i;

  // Configuring pins for peripheral/crystal usage
  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

  //set the internal reference for what the frequency fo the external crystals are
  CS_setExternalClockSourceFrequency(EXTERNAL_LOW_FREQUENCY_CRYSTAL_FREQ, EXTERNAL_HIGH_FREQUENCY_CRYSTAL_FREQ);

  // Starting HFXT and LFXT in non-bypass mode without a timeout. Before we start we have to change VCORE to 1 to support the 48MHz frequency
  PCM_setCoreVoltageLevel(PCM_VCORE1);
  hfxtGood = CS_startHFXTWithTimeout(false, 1000);
  lfxtGood = CS_startLFXTWithTimeout(CS_LFXT_DRIVE0, 1000);

  // Initializing clocks
  //main clock set to use dco clock or HF external clock, 48Mhz which is its max. Used by CPU
  //sub-main clock set to use HF external clock or dco clock, 24Mhz which is its max. Used by periphs.
  //low-speed submain uses same clock as sub-main. Used by periphs
  //aux and backup clocks also can be used by periphs. Set to REFO.
  //set REFO to 128Khz since peripherals can divide it down from there themselves if they want a lower clock speed.
  //Also, if the HFXT doesn't work, clocks that use it will default to using SYSOSC instead (5 Mhz)
  CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
  if(hfxtGood == false)
  {
    CS_setDCOFrequency(DCOMaxFreq);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
  }
  else
  {
    CS_setDCOFrequency(3000000); //set DCO to default of 3 Mhz
    CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_2);
  }
  CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
  CS_initClockSignal(CS_BCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
  for(i = 0; i < 10000; i++);

  //
  //  SysTick is used for delay() and delayMicroseconds() and micros and millis
  //
  SysTick_enableModule();
  SysTick_setPeriod(CS_getMCLK() / 1000); //uses MCLK. Hz (ticks/s) * 1 ms = ticks needed to make it time out in 1 millisecond
  SysTick_registerInterrupt(SysTickIntHandler);
  Interrupt_setPriority(FAULT_SYSTICK, SystickIntPriority);
  SysTick_enableInterrupt();
  Interrupt_enableMaster();

}

uint32_t setCpuClockFreq(uint32_t newFrequency)
{
  if(newFrequency > CpuMaxFreq)
  {
    newFrequency = CpuMaxFreq;
  }
  return setClockFreq(newFrequency, CS_MCLK);
}

uint32_t setPeriphClockFreq(uint32_t newFrequency)
{
  if(newFrequency > PeriphMaxFreq)
  {
    newFrequency = PeriphMaxFreq;
  }
  return setClockFreq(newFrequency, CS_HSMCLK);
}

uint32_t setProgrammableClockSourceFreq(uint32_t newFrequency)
{
  if(newFrequency > DCOMaxFreq)
  {
    newFrequency = DCOMaxFreq;
  }
  CS_setDCOFrequency (newFrequency);
  return CS_getDCOFrequency();
}

uint32_t getCpuClockFreq()
{
  return(CS_getMCLK());
}

uint32_t getPeriphClockFreq()
{
  return(CS_getHSMCLK());
}

uint32_t micros(void)
{
  uint32_t microSeconds;
  uint32_t ticksSinceRollover;

  microSeconds = milliseconds * 1000;

  ticksSinceRollover = (SysTick_getPeriod() - SysTick_getValue()) & 0xFFFFFF; //24 bit counter, counts downward.
  microSeconds += ((ticksSinceRollover * 1000)/ SysTick_getPeriod()); //(ticks / ticks per millisecond) * 1000 to get microseconds

  return microSeconds;

}

uint32_t millis(void)
{
  return milliseconds;
}

void delayMicroseconds(uint32_t microsToDelay)
{
  // Systick timer rolls over every 1000000/SYSTICKHZ microseconds
  if(microsToDelay >= 1000){
    delay(microsToDelay / 1000);  // delay milliseconds
    microsToDelay = microsToDelay % 1000;     // handle remainder of delay
  };

  // 24 bit timer - mask off undefined bits
  unsigned long startTime = SysTick_getValue();

  unsigned long ticks = microsToDelay * (CS_getMCLK()/1000000UL);
  volatile unsigned long elapsedTime;

  if (ticks > startTime) {
    ticks = (ticks + (0xFFFFFF - (CS_getMCLK() / SystickHz))) & 0xFFFFFF;
  }

  do {
    elapsedTime = (startTime-SysTick_getValue()) & 0xFFFFFF;
  } while(elapsedTime <= ticks);
}

void delay(uint32_t millis)
{
  uint32_t startTime = milliseconds;
  while(millis > (milliseconds - startTime));
}

static void SysTickIntHandler(void)
{

  milliseconds++;
}

static bool needChangeVoltageStateFirst(uint32_t desiredCpuFreq, uint32_t desiredPeriphFreq)
{
  uint32_t presentCpuFreq = CS_getMCLK();
  uint32_t presentPeriphFreq = CS_getHSMCLK();

  //If either clock is operating in the range of minimum power mode but is about to transition to the range used by high power mode, then
  //we need to change power states first.
  //If either clock is operating in the range of maximum power mode already, then the clocks are free to change before we try moving to a different
  //power level since high power mode can support all frequencies.
  if((desiredCpuFreq > MinVoltageMode_CpuMaxFreq && presentCpuFreq <= MinVoltageMode_CpuMaxFreq) || (desiredPeriphFreq > MinVoltageMode_PeriphMaxFreq && presentPeriphFreq <= MinVoltageMode_PeriphMaxFreq))
  {
    return true;
  }
  else
  {
    return false;
  }
}

static void selectOptimalVoltageState(uint32_t cpuFreq, uint32_t periphFreq)
{
  if(cpuFreq <= MinVoltageMode_CpuMaxFreq && periphFreq <= MinVoltageMode_PeriphMaxFreq)
  {
    PCM_setCoreVoltageLevel(PCM_VCORE0);
  }
  else
  {
    PCM_setCoreVoltageLevel(PCM_VCORE1);
  }
}

static uint32_t setClockFreq(int64_t newFrequency, uint32_t clockToSet)
{
  uint32_t mainClockSourceFreq;
  uint32_t mainClockSource;
  int64_t hfxtComputedFreq;
  uint32_t dcoComputedFreq;
  uint32_t firmwareDivisor;
  uint32_t cpuFreq;
  uint32_t periphFreq;
  uint8_t literalDivisor;
  uint8_t i;

  //figure out what clock source we need to be using for the requested range
  if(newFrequency <= REFOCLK_FREQ && newFrequency >= EXTERNAL_LOW_FREQUENCY_CRYSTAL_FREQ)
  {
    mainClockSourceFreq = REFOCLK_FREQ; //if we can use the refo or the lfxt, just use it instead of the others.
    mainClockSource = CS_REFOCLK_SELECT;
  }
  else if((newFrequency <= EXTERNAL_LOW_FREQUENCY_CRYSTAL_FREQ) && lfxtGood)
  {
    mainClockSourceFreq = EXTERNAL_LOW_FREQUENCY_CRYSTAL_FREQ;
    mainClockSource = CS_LFXTCLK_SELECT;
  }

  //wasn't compatible with one of the lower power clocks, try a higher one
  else if(hfxtGood == false)
  {
    mainClockSourceFreq = CS_getDCOFrequency();
    mainClockSource = CS_DCOCLK_SELECT;
  }
  else if(newFrequency < HFXTCLK_MINIMUM && newFrequency >= DCOCLK_MINIMUM)
  {
    mainClockSourceFreq = CS_getDCOFrequency();
    mainClockSource = CS_DCOCLK_SELECT;
  }
  else if(newFrequency >= HFXTCLK_MINIMUM && newFrequency < DCOCLK_MINIMUM)
  {
    mainClockSourceFreq = EXTERNAL_HIGH_FREQUENCY_CRYSTAL_FREQ;
    mainClockSource = CS_HFXTCLK_SELECT;
  }
  else //either DCO or HFXT will work. Let's figure out which would be more accurate
  {
    mainClockSourceFreq = CS_getDCOFrequency();
    for(i = 1; i < 128; i *= 2)
    {
      int temp = mainClockSourceFreq / i;
      if(temp <= newFrequency)
      {
        dcoComputedFreq = temp;
        break;
      }
    }

    mainClockSourceFreq = EXTERNAL_HIGH_FREQUENCY_CRYSTAL_FREQ;
    for(i = 1; i < 128; i *= 2)
    {
      int temp = mainClockSourceFreq / i;
      if(temp <= newFrequency)
      {
        hfxtComputedFreq = temp;
        break;
      }
    }

    if(abs(hfxtComputedFreq - newFrequency) > abs(dcoComputedFreq - newFrequency))
    {
      mainClockSourceFreq = CS_getDCOFrequency();
      mainClockSource = CS_DCOCLK_SELECT;
    }
    else
    {
      mainClockSourceFreq = EXTERNAL_HIGH_FREQUENCY_CRYSTAL_FREQ;
      mainClockSource = CS_HFXTCLK_SELECT;
    }
  }

  //figure out which divisor we need to use for the requested range
  if(newFrequency >= mainClockSourceFreq)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_1;
    literalDivisor = 1;
  }
  else if(newFrequency >= mainClockSourceFreq / 2)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_2;
    literalDivisor = 2;
  }
  else if(newFrequency >= mainClockSourceFreq / 4)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_4;
    literalDivisor = 4;
  }
  else if(newFrequency >= mainClockSourceFreq / 8)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_8;
    literalDivisor = 8;
  }
  else if(newFrequency >= mainClockSourceFreq / 16)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_16;
    literalDivisor = 16;
  }
  else if(newFrequency >= mainClockSourceFreq / 32)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_32;
    literalDivisor = 32;
  }
  else if(newFrequency >= mainClockSourceFreq / 64)
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_64;
    literalDivisor = 64;
  }
  else
  {
    firmwareDivisor = CS_CLOCK_DIVIDER_128;
    literalDivisor = 128;
  }

  //Compute the desired and achievable new frequencies
  if(clockToSet == CS_MCLK)
  {
    cpuFreq = mainClockSourceFreq / literalDivisor;
    periphFreq = CS_getHSMCLK();
  }
  else if(clockToSet == CS_HSMCLK)
  {
    cpuFreq = CS_getMCLK();
    periphFreq = mainClockSourceFreq / literalDivisor;
  }

  //configure clock to new setting, but be careful to make sure the power settings
  //will allow it. Either way, optimize power settings at the same time.
  if(needChangeVoltageStateFirst(cpuFreq,periphFreq))
  {
    selectOptimalVoltageState(cpuFreq, periphFreq);
    CS_initClockSignal(clockToSet, mainClockSource, firmwareDivisor);
  }
  else
  {
    CS_initClockSignal(clockToSet, mainClockSource, firmwareDivisor);
    selectOptimalVoltageState(cpuFreq, periphFreq);
  }

  for(i = 0; i < 100; i++);

  if(clockToSet == CS_MCLK)
  {
    SysTick_setPeriod(CS_getMCLK() / 1000); //uses MCLK. Hz (ticks/s) * 1 ms = ticks needed to make it time out in 1 millisecond
    return CS_getMCLK();
  }
  else if(clockToSet == CS_HSMCLK)
  {
    return CS_getHSMCLK();
  }
  else
  {
    debugFault("SetClockFreq error: Clock not supported");
    return 0;
  }
}
