/*
 * RovePermaMem_Private.h
 *
 *  Created on: Oct 22, 2017
 *      Author: drue
 */

//stuff in here is not supposed to be used by the user. Sysinit needs some of these so
//broken out into a separate internal include file instead

#ifndef ROVEBOARD_TM4C1294NCPDT_API_ROVEPERMAMEM_ROVEPERMAMEM_INTERNAL_H_
#define ROVEBOARD_TM4C1294NCPDT_API_ROVEPERMAMEM_ROVEPERMAMEM_INTERNAL_H_

//sets up the eeprom for usage by rovePermaMem. Performs duties such as initializing the global block tables,
//setting up the eeprom for first time usage if necessary, and locking it after initialization is finished
void rovePermaMem_Init();

#endif /* ROVEBOARD_TM4C1294NCPDT_API_ROVEPERMAMEM_ROVEPERMAMEM_INTERNAL_H_ */
