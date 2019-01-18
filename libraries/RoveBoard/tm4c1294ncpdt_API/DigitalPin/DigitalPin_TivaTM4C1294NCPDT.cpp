#include "tm4c1294ncpdt_API/DigitalPin/DigitalPin_TivaTM4C1294NCPDT.h"
#include "../tivaware/driverlib/gpio.h"
#include "../tivaware/driverlib/sysctl.h"
#include "../tivaware/inc/hw_gpio.h"
#include "../tivaware/inc/hw_memmap.h"

static const uint8_t MaxPower = MA_12;
static const uint8_t MinPower = MA_2;
static bool validatePin(uint8_t pinNumber, uint8_t * pinMask, uint32_t *portBase);

const uint8_t InvalidPinCount = 1;
const uint8_t InvalidPins[InvalidPinCount] = {PD_7}; //locked or used by JTAG

static uint32_t constToPinStrength[] = {GPIO_STRENGTH_2MA, GPIO_STRENGTH_4MA, GPIO_STRENGTH_6MA, GPIO_STRENGTH_8MA, GPIO_STRENGTH_10MA, GPIO_STRENGTH_12MA};
 

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
        GPIOPinTypeGPIOInput(portBase, pinMask);
        break;
      
      case PullUpInput:
        GPIODirModeSet(portBase, pinMask, GPIO_DIR_MODE_IN);
        GPIOPadConfigSet(portBase, pinMask, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //strength doesn't matter on input
        break;
        
      case PullDownInput:
        GPIODirModeSet(portBase, pinMask, GPIO_DIR_MODE_IN);
        GPIOPadConfigSet(portBase, pinMask, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD); //strength doesn't matter on input
        break;
        
      default:
        GPIOPinTypeGPIOInput(portBase, pinMask);
        break;
    }
    
    if(GPIOPinRead(portBase, pinMask))
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
  return digitalPinWritePower(pinNumber, outputLevel, outputMode, MA_2);
}

bool digitalPinWritePower(uint8_t pinNumber, bool outputLevel, uint8_t outputMode, uint8_t outputPowerLevel)
{
  uint8_t pinMask;
  uint32_t portBase;
  uint8_t outputMask;
  
  if(MinPower <= outputPowerLevel && outputLevel <= MaxPower)
  {
    if(validatePin(pinNumber, &pinMask, &portBase))
    {
      //set up pin for writing mode
      switch(outputMode)
      {
        case Output:
          GPIODirModeSet(portBase, pinMask, GPIO_DIR_MODE_OUT);
          GPIOPadConfigSet(portBase, pinMask, constToPinStrength[outputPowerLevel], GPIO_PIN_TYPE_STD); 
          break;
        
        case OpenDrainOutput:
          GPIODirModeSet(portBase, pinMask, GPIO_DIR_MODE_OUT);
          GPIOPadConfigSet(portBase, pinMask, constToPinStrength[outputPowerLevel], GPIO_PIN_TYPE_OD); 
          break;
          
        default:
          GPIOPinTypeGPIOOutput(portBase, pinMask);
          break;
      }
      outputMask = outputLevel ? pinMask : 0;
      GPIOPinWrite(portBase, pinMask, outputMask);
      
      return(true);
    }
  }
  
  return (false);
}

//checks to see if the pin the user passed is valid for digital operation.
//returns true or false, if it was or wasn't.
//Returns by reference the pin's mask and the pin's port base, for use in the
//firmware calls
static bool validatePin(uint8_t pinNumber, uint8_t * pinMask, uint32_t *portBase)
{
  if(pinNumber > 95)
  {
    return(false); //not a pin
  }
  
  for(int i = 0; i < InvalidPinCount; i++) //make sure it's not an invalid pin
  {
    if(pinNumber == InvalidPins[i])
    {
      return (false);
    }
  }
  
  *pinMask = pinToPinMask[pinNumber];
  
  if(*pinMask == NotAPin)
  {
    return(false); //also not a pin
  }
  
  uint8_t portRef = pinToPortRef[pinNumber];
  *portBase = portRefToPortBase[portRef];
  
  return(true);
}
