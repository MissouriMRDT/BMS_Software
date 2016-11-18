/*
 * @file fans.c
 * @author Jesse Cureton
 * @date 28 June 2016
 *
 * @brief Fan control logic + soft PWM implementation for battery box fans
 */

#include "main.h"
#include "fans.h"
#include "drivers/mcp23s.h"

/**
 * Runs every main loop to service fan PWM counts
 */
void serviceFanPWM()
{
	for(uint8_t i = 0; i < NUM_FANS; i++)
	{
		//If at start, turn the fan on
		if(fanCurrentCount[i] == 0) setFanState(i, TRUE);

		//If at the setpoint, turn the fan off
		if(fanCurrentCount[i] == fanSetPoint[i]) setFanState(i, FALSE);

		//Increment the count, rolling over if necessary
		fanCurrentCount[i]++;
		if(fanCurrentCount[i] == 5) fanCurrentCount[i] = 0;
	}
}

/**
 * Set the PWM duty cycle for all fans, or any individual fan.
 * @param channel Which fan to toggle, one of the {@link FAN_ID_VALUES fan id} values. Use {@link NUM_FANS} to set all channels
 * @param speed One of the {@link FAN_SPEED_VALUES fan speeds}.
 */
void setFanSpeed(uint8_t channel, uint8_t speed)
{
	if(channel == NUM_FANS)
	{
		for(uint8_t i = 0; i < NUM_FANS; i++)
		{
			fanSetPoint[i] = speed;
		}
	} else {
		fanSetPoint[channel] = speed;
	}
}

/**
 * Enable or disable a fan.
 * @param channel Which fan to toggle, one of the {@link FAN_ID_VALUES} values
 * @param isEnabled {@link TRUE} or {@link FALSE}
 */
void setFanState(uint8_t channel, uint8_t isEnabled)
{
	switch(channel)
	{
		case FAN_1:
			if(isEnabled) OD_OUTPUTS |= OD_FAN1;
			else OD_OUTPUTS &= ~OD_FAN1;
			break;
		case FAN_2:
			if(isEnabled) OD_OUTPUTS |= OD_FAN2;
			else OD_OUTPUTS &= ~OD_FAN2;
			break;
		case FAN_3:
			if(isEnabled) OD_OUTPUTS |= OD_FAN3;
			else OD_OUTPUTS &= ~OD_FAN3;
			break;
		case FAN_4:
			if(isEnabled) OD_OUTPUTS |= OD_FAN4;
			else OD_OUTPUTS &= ~OD_FAN4;
			break;
		default:
			break;
	}
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
}



