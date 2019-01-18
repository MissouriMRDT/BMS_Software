/*
 * RoveBoard_MSP432P401R.h
 *
 *  Created on: Oct 15, 2017
 *      Author: drue
 *
 *  Primary external include for main.cpp and similar, non-roveware files, whenever the board being used is the
 *  MSP432P401R
 */

#ifndef ROVEBOARD_ROVEBOARD_MSP432P401R_H_
#define ROVEBOARD_ROVEBOARD_MSP432P401R_H_

#ifndef __MSP432P401R__
#define __MSP432P401R__
#endif

#include "msp432p401r_API/SysInit/SysInit_MSP432P401R.h"
static int dummySoThisRunsBeforeMain = (initSystem(), 0);

#include "supportingUtilities/RoveUtilities.h"
#include "supportingUtilities/Debug.h"
#include "msp432p401r_API/Clocking/Clocking_MSP432P401R.h"
#include "msp432p401r_API/Power/Power_MSP432P401R.h"
#include "msp432p401r_API/RoveUart/RoveUart_MSP432P401R.h"
#include "msp432p401r_API/RovePinMap_MSP432P401R.h"
#include "msp432p401r_API/DigitalPin/DigitalPin_MSP432P401R.h"
#include "supportingUtilities/lightTrig.h"
#include "supportingUtilities/MatrixMath.h"

#endif /* ROVEBOARD_ROVEBOARD_MSP432P401R_H_ */
