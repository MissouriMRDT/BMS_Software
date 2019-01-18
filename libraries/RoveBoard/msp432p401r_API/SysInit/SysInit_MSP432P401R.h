/*
 * SysInit.h
 *
 *  Created on: Oct 15, 2017
 *      Author: drue
 */

#ifndef ROVEBOARD_MSP432P401R_API_SYSINIT_SYSINIT_MSP432P401R_H_
#define ROVEBOARD_MSP432P401R_API_SYSINIT_SYSINIT_MSP432P401R_H_

/**
 * Initialize the system
 *
 * Performs the following initialization steps:
 *     1. Enables the FPU
 *     2. Halts the Watch dog timer
 *     3. Enables all SRAM banks
 *     4. Sets up power regulator and VCORE
 *     5. Enable Flash wait states if needed
 *     6. Sets up system clocks
 *     7. Enable Flash read buffering
 */
void initSystem();


#endif /* ROVEBOARD_MSP432P401R_API_SYSINIT_SYSINIT_MSP432P401R_H_ */
