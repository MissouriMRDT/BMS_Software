#include "SysInit_TivaTM4C1294NCPDT.h"
#include "../Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "../RovePermaMem/RovePermaMem_Internal.h"
#include "supportingUtilities/Debug.h"
#include "../tivaware/driverlib/rom.h"
#include "../tivaware/driverlib/sysctl.h"
#include "../tivaware/driverlib/fpu.h"


static bool initialized = false;

void initSystem()
{
  if(!initialized)
  {

    //Floating point computational unit is on by default in some compilers,
    //but put it here anyway just in case we use one that doesn't
    FPUEnable();
    FPULazyStackingEnable();

    //turn on system main clocks and system tick (which is used by things like delay and ethernet),
    //and turn on all GPIO ports as those are pretty much always used
    initSystemClocks();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);

    //turn on any roveboard stuff than needs pre-main initialization
    rovePermaMem_Init();

    //let peripherals finish turning on
    delayMicroseconds(100);

    initialized = true;
  }
}
