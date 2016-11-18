/**
 * @file interrupts.c
 *
 * Battery management system interrupt handlers for Port 1 & timer interrupts
 *
 * @author Jesse Cureton
 */

#include <msp430.h>
#include "main.h"
#include "pins.h"
#include "can.h"

// Port 1 interrupt service routine
//If we end up switching later to the GNU compiler, use this:
//	void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
/**
 * Port 1 interrupt handler.
 *
 * Check if we received an interrupt on the CAN_INT pin,
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	//Check if it was actually a CAN interrupt
	if(P1IFG & CAN_INT)
	{
		if((P2OUT & OPENDRAIN_CS) == 0 || (P3OUT & LED_CS) == 0)
		{
			//There is a SPI transmission in progress.
			while((UCB0STAT & UCBUSY) != 0); //Wait for TX/RX cycle to complete
			P2OUT |= OPENDRAIN_CS;
			P3OUT |= LED_CS;
		} else if(CAN_BUSY == 0) {
			//Ensure we don't interrupt another CAN operation
			can_receive();
		}
		P1IFG &= ~CAN_INT;
	}
}


// Timer0_A0 Interrupt Vector (TAIV) handler
/**
 * Timer 0 Interrupt handler.
 *
 * Toggle the heartbeat LED, determine the state we enter after waking from sleep.
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	P1OUT^=BIT0;				//Heartbeat LED
	if(state != STATE_IDLE) return;	//Don't change state unless we're idle
	else if(STATUS & STAT_TRIPPED) state = STATE_ISOLATED;
	else state = STATE_MEASURE;
	LPM3_EXIT;						// exit LPM3
}
