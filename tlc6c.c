/*
 * tlc6c.c
 *
 *  Created on: Apr 13, 2017
 *      Author: tef
 */

#include "tlc6c.h"
#include "delay.h"
#include "msp432p401r.h"

void tlc6c_write_bit(int bit)
{

    if(bit)
        P8OUT |= LED_SER_IN;
    else
        P8OUT &= ~LED_SER_IN;
    P9OUT |= LED_SRCK;
    DELAY_US(10);
    P9OUT &= ~LED_SRCK;
    DELAY_US(10);
}

void tlc6c_write_byte(int byte)
{
    int i=0;
    for(i=0; i<8; i++)
        tlc6c_write_bit((byte >> i) & 0x01);
    P8OUT |= LED_RCK;
    DELAY_US(10);
    P8OUT &= ~LED_RCK;

}
