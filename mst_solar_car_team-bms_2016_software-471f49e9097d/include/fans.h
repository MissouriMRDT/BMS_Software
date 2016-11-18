/*
 * @file fans.h
 * @author Jesse Cureton
 * @date 28 June 2016
 *
 * @brief Fan control logic + soft PWM implementation for battery box fans
 */

#ifndef INCLUDE_FANS_H_
#define INCLUDE_FANS_H_

#include "main.h"

/**
 * @defgroup FAN_ID_VALUES Fan IDs
 *
 * These values can be used to enumerate fans.
 *
 * @{
 */
#define FAN_1		0	///< Leftmost fan from the perspective of the driver
#define FAN_2		1	///< Second leftmost fan from the perspective of the driver
#define FAN_3		2	///< Second rightmost fan from the perspective of the driver
#define FAN_4		3	///< Rightmost fan from the perspective of the driver
/** @} */
#define NUM_FANS	4	///< The number of fans connected to the system

/**
 * @defgroup FAN_SPEED_VALUES Fan speeds
 *
 * These values describe fan speeds
 *
 * @{
 */
#define FAN_SPEED_0 	0	///< 0% fan speed - off.
#define FAN_SPEED_1 	1	///< 20% fan speed
#define FAN_SPEED_2 	2	///< 40% fan speed
#define FAN_SPEED_3 	3	///< 60% fan speed
#define FAN_SPEED_4 	4	///< 80% fan speed
#define FAN_SPEED_5 	5	///< 100% fan speed
/** @} */

uint8_t fanCurrentCount[NUM_FANS];
uint8_t fanSetPoint[NUM_FANS];

//Functions
void setFanSpeed(uint8_t channel, uint8_t speed);
void setFanState(uint8_t channel, uint8_t isEnabled);
void serviceFanPWM();

#endif /* INCLUDE_FANS_H_ */
