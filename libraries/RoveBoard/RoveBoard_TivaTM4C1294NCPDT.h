#ifndef ROVEBOARD_TIVATM4C1294NCPDT_H_
#define ROVEBOARD_TIVATM4C1294NCPDT_H_

/*
 * RoveBoard_TivaTM4C1294NCPDT.h
 *
 *  Created on: Sep 4, 2017
 *      Author: drue
 *
 *  Primary external include for main.cpp and similar, non-roveware files, whenever the board being used is the
 *  tiva tm4c1294ncpdt
 */

#ifndef PART_TM4C1294NCPDT
#define PART_TM4C1294NCPDT
#endif

//sysinit always before anything else
#include "tm4c1294ncpdt_API/SysInit/SysInit_TivaTM4C1294NCPDT.h"
static int dummySoThisRunsBeforeMain = (initSystem(), 0);

#include "supportingUtilities/RoveUtilities.h"
#include "supportingUtilities/Debug.h"
#include "supportingUtilities/lightTrig.h"
#include "supportingUtilities/MatrixMath.h"
#include "tm4c1294ncpdt_API/Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/DigitalPin/DigitalPin_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RovePwmRead/RovePwmRead_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RovePwmWrite/RovePwmWrite_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RoveEthernet/RoveEthernet_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RovePinMap_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RoveTimer/RoveTimer_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RoveUart/RoveUart_TivaTm4c1294NCPDT.h"
#include "tm4c1294ncpdt_API/RoveI2C/RoveI2C_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RovePermaMem/RovePermaMem_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RoveADC/RoveADC_TivaTM4C1294NCPDT.h"
#include "tm4c1294ncpdt_API/RoveWatchdog/RoveWatchdog_TivaTM4C1294NCPDT.h"

#endif
