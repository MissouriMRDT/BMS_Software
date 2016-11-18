/**
 * @file relays.h
 *
 * @author Jesse Cureton
 * @date 27 April 2016
 */

#ifndef INCLUDE_RELAYS_H_
#define INCLUDE_RELAYS_H_

volatile uint8_t RELAY_EVENTS;				///< The global-scope bitfield describing active relay events, with bits defined as in {@link RELAY_EVENT_BITS}.
/**
 * @defgroup RELAY_EVENT_BITS Relay Events
 *
 * These bits describe what events are active in the relay subsystem.
 *
 * @{
 */
#define EVNT_SERVICE_PRECHARGE BIT0	///< The precharge relay has been activated and must be soon switched over to the motor relay.
/** @} */

volatile uint8_t runsSincePrechargeOn;	///< Used to count how long it has been since precharge came on, so we know when to enable the regular motor relay and disable precharge

void serviceRelays();
void connectMotor();
void disconnectMotor();
void connectGround();
void disconnectGround();
void isolatePack();


#endif /* INCLUDE_RELAYS_H_ */
