/*
 * Debug.h
 *
 *  Created on: Oct 4, 2017
 *      Author: drue
 *
 *  Contains functions useful for debugging, such as software fault loops
 */

#ifndef ROVEBOARD_UTILITIES_DEBUG_H_
#define ROVEBOARD_UTILITIES_DEBUG_H_

#ifdef __cplusplus
extern "C"
{
#endif
//enters into an infinite loop, carrying a message about what caused the fault to happen.
//note: You can avoid this by putting #define ROVEDEBUG_NO_DEBUG at the top of main
void debugFault(const char msg[]);

#ifdef __cplusplus
}
#endif
#endif /* ROVEBOARD_SUPPORTINGUTILITIES_DEBUG_H_ */
