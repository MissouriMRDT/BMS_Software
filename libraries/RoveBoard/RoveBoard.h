/*
 * RoveBoard.h
 *
 *  Created on: Sep 4, 2017
 *      Author: drue
 *
 *  Primary external include for roveware files like RoveComm. Main.cpp and related files shouldn't
 *  call this, on the other hand, but instead call at the program beginning the version of RoveBoard
 *  associated with the chip the program is being ran on.
 */

#ifndef ROVEBOARD_ROVEBOARD_H_
#define ROVEBOARD_ROVEBOARD_H_

#include "standardized_API/Clocking.h"
#include "standardized_API/DigitalPin.h"
#include "standardized_API/RoveEthernet.h"
#include "standardized_API/RoveI2C.h"
#include "standardized_API/RovePwmRead.h"
#include "standardized_API/RovePwmWrite.h"
#include "standardized_API/RoveTimer.h"
#include "standardized_API/RoveUart.h"
#include "standardized_API/RovePermaMem.h"
#include "standardized_API/RoveADC.h"
#include "standardized_API/RoveWatchdog.h"
#include "supportingUtilities/RoveUtilities.h"
#include "supportingUtilities/Debug.h"
#include "supportingUtilities/lightTrig.h"
#include "supportingUtilities/MatrixMath.h"


#endif /* ROVEBOARD_ROVEBOARD_H_ */
