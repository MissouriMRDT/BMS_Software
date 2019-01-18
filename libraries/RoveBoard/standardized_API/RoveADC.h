/*
 * RoveADC.h
 *
 *  Created on: Oct 27, 2017
 *      Author: drue
 */

#ifndef ROVEBOARD_STANDARDIZED_API_ROVEADC_H_
#define ROVEBOARD_STANDARDIZED_API_ROVEADC_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct RoveAdc_Handle
{
  bool initialized;
  uint16_t index;

#ifdef __cplusplus
  RoveAdc_Handle()
  {
    initialized = false;
  }
#endif

} RoveAdc_Handle;

typedef enum RoveAdc_Error
{
  ROVEADC_ERROR_UNKNOWN,
  ROVEADC_INCOMPLETE_CONVERSION,
  ROVEADC_SUCCESS
} RoveAdc_Error;

//Overview: initialize the adc module to read analog voltages off of a certain GPIO pin with the default settings.
//          Default settings and proper inputs are board-specific.
//
//returns:  An initialized adc instance
//Warning:  Function will go into an infinite debug fault routine if the module or the pin aren't valid, unless rove debug is disabled.
extern RoveAdc_Handle roveAdc_init(uint16_t module, uint8_t pin);

//Overview: Begins the process of reading an analog voltage with the given adc instance.
//Inputs:   An initialized adc instance, given from roveAdc_init
//returns:  any errors found
extern RoveAdc_Error roveAdc_getConvResults(RoveAdc_Handle handle, uint32_t *retBuff);

//Overview: Gets the raw data found after a conversion has been completed with the given adc instance.
//Inputs:   An initialized adc instance, given from roveAdc_init, and a return-by-pointer variable that will contain the conversion data on return.
//returns:  ROVEADC_SUCCESS if data was successfully gotten, or ROVEADC_INCOMPLETE_CONVERSION if either the conversion is still in progress or
//          if you never called startConversion before this in the first place.
extern RoveAdc_Error roveAdc_startConversion(RoveAdc_Handle handle);

//Converts raw data returned from getConvResults into volts, represented as a float.
extern float roveAdc_toVolts(uint32_t result);

#endif /* ROVEBOARD_STANDARDIZED_API_ROVEADC_H_ */
