/*
 * Programmer: Drue Satterfield
 * Date o creation: ~9/4/17
 *
 * General, non-hardware-specific list of pwm reading functions that each board in the RoveBoard network supports.
 */

#ifndef PWMREADER_H_
#define PWMREADER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct RovePwmRead_Handle
{
  bool initialized;
  uint16_t mappedPin;
  uint16_t index;

#ifdef __cplusplus
  RovePwmRead_Handle()
  {
    initialized = false;
  }
#endif
} RovePwmRead_Handle;

typedef enum RovePwmRead_Scale {PWM_NANO, PWM_MICRO} RovePwmRead_Scale;

//Begins reading pwm pulses on the specified pin using the specified timer.
//Input: The pwmRead module to use, and which of its associated GPIO pins are to be used
//Returns a rovePwmRead handle with internal settings initialized
//warning: if the arguments are invalid, the function enters an infinite loop fault routine for checking in a debugger
extern RovePwmRead_Handle initPwmRead(uint8_t readingModule, uint8_t mappedPin);

//Stops reading pwm.
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
extern void stopPwmRead(RovePwmRead_Handle handle);

//gets the duty cycle being read on the specified pin.
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
//Output: 0-100 duty cycle
extern uint8_t getDuty(RovePwmRead_Handle handle);

//gets the total period of the PWM signal last transmitted for
//the specified pin
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
//Output: period of last transmission
extern uint32_t getTotalPeriod(RovePwmRead_Handle handle, RovePwmRead_Scale scale);

//Gets the on period of the last tramsittted PWM signal for
//the specified pin
//Input: The handle for the pwm reading instance to stop reading with
//Note: initPwmRead must be called before hand
//Output: On-period of pulse
extern uint32_t getOnPeriod(RovePwmRead_Handle handle, RovePwmRead_Scale scale);

#endif
