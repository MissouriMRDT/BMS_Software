/**
 * @file relays.c
 *
 * @author Jesse Cureton
 * @date 27 April 2016
 */
#include "main.h"
#include "config.h"
#include "relays.h"
#include "drivers/mcp23s.h"

/**
 * Process any relay events that have been marked as active.
 *
 * This function handles the implementation of some of the timing-dependent things related to relays, such as switching from precharge relay to motor relay once precharge has been activated.
 */
void serviceRelays()
{
	if(STATUS & STAT_TRIPPED)
	{
		isolatePack();
		return;	//Never change a relay if we are isolated
	}

	//Check if we have an active precharge event
	if(RELAY_EVENTS & EVNT_SERVICE_PRECHARGE)
	{
		if(runsSincePrechargeOn >= (PRECHARGE_DELAY / 200)) //Main loop is 200ms, so ms/200 gives us iterations
		{
			//Turn on the motor relay, ensure that it has time to switch on, then disable precharge
			OD_OUTPUTS |= OD_MOTOR;
			mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
			delayMillis(15);	//From contactor datasheet - takes 15ms to fully close
			OD_OUTPUTS &= ~OD_PRECHARGE;
			mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
			RELAY_EVENTS &= ~EVNT_SERVICE_PRECHARGE;
		}
		runsSincePrechargeOn++;
	}

	if(STATUS & STAT_CHARGE)
	{
		OD_OUTPUTS |= OD_ARRAY;
		mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
	} else
	{
		OD_OUTPUTS &= ~OD_ARRAY;
		mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);

	}
}

/**
 * Begin the process of turning on the motor relay by marking a precharge event as active.
 */
void connectMotor()
{
	//Don't connect if there's an active precharge event or the motor is already on
	if((RELAY_EVENTS & EVNT_SERVICE_PRECHARGE) || (OD_OUTPUTS & OD_MOTOR)) return;

	runsSincePrechargeOn = 0;

	RELAY_EVENTS |= EVNT_SERVICE_PRECHARGE; //Mark that we must service the precharge relay
	OD_OUTPUTS |= OD_PRECHARGE;				//Enable the precharge open drain output
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
}

/**
 * Disconnect the motor and precharge relays.
 */
void disconnectMotor()
{
	OD_OUTPUTS &= ~(OD_MOTOR + OD_PRECHARGE);
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
}

/**
 * Disconnect all relays and isolate the pack.
 */
void isolatePack()
{
	OD_OUTPUTS &= ~(OD_COMMON_GND + OD_PRECHARGE + OD_MOTOR + OD_ARRAY);
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
	//STATUS |= STAT_TRIPPED;
}

/**
 * Connect the common ground relay.
 */
void connectGround()
{
	//Don't do anything if already connected
	if(OD_OUTPUTS & OD_COMMON_GND) return;

	OD_OUTPUTS |= OD_COMMON_GND;
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
}

/**
 * Disconnect the common ground relay
 */
void disconnectGround()
{
	OD_OUTPUTS &= ~OD_COMMON_GND;
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, OD_OUTPUTS);
}
