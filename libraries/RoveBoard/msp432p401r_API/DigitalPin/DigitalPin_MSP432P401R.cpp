#include "DigitalPin_MSP432P401R.h"
#include "../msp432ware/driverlib/driverlib.h"

static bool validatePin(uint8_t pinNumber, uint8_t * pinMask, uint32_t *portBase);

bool digitalPinRead(uint8_t pinNumber)
{
  return digitalPinReadMode(pinNumber, Input);
}

bool digitalPinReadMode(uint8_t pinNumber, uint8_t inputMode)
{
  uint8_t pinMask;
  uint32_t portBase;
  
  if(validatePin(pinNumber, &pinMask, &portBase))
  {
    //set up pin for reading mode
    switch(inputMode)
    {
      case Input:
        GPIO_setAsInputPin(portBase, pinMask);
        break;
      
      case PullUpInput:
        GPIO_setAsInputPinWithPullUpResistor(portBase, pinMask);
        break;
        
      case PullDownInput:
        GPIO_setAsInputPinWithPullDownResistor(portBase, pinMask);
        break;
        
      default:
        GPIO_setAsInputPin(portBase, pinMask);
        break;
    }
    
    if(GPIO_getInputPinValue(portBase, pinMask) == GPIO_INPUT_PIN_HIGH)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return(false);
  }
}

bool digitalPinWrite(uint8_t pinNumber, bool outputLevel)
{
  return digitalPinWriteMode(pinNumber, outputLevel, Output);
}

bool digitalPinWriteMode(uint8_t pinNumber, bool outputLevel, uint8_t outputMode)
{
  if(outputMode != Output)
  {
    return false;
  }

  uint8_t pinMask;
  uint32_t portBase;
  
  if(validatePin(pinNumber, &pinMask, &portBase))
  {
    //set up pin for writing mode
    GPIO_setAsOutputPin(portBase, pinMask);

    if(outputLevel == HIGH)
    {
      GPIO_setOutputHighOnPin(portBase, pinMask);
    }
    else
    {
      GPIO_setOutputLowOnPin(portBase, pinMask);
    }

    return(true);
  }
  
  return (false);
}

//checks to see if the pin the user passed is valid for digital operation.
//returns true or false, if it was or wasn't.
//Returns by reference the pin's mask and the pin's port base, for use in the
//firmware calls
static bool validatePin(uint8_t pinNumber, uint8_t * pinMask, uint32_t *portBase)
{
  if(pinNumber > 100)
  {
    return(false); //not a pin
  }
  
  *pinMask = pinToPinMask[pinNumber];
  
  if(*pinMask == NotAPin)
  {
    return(false); //also not a pin
  }
  
  *portBase = pinToPinPort[pinNumber];
  if(*portBase == NotAPin)
  {
    return(false);
  }
  
  return(true);
}
