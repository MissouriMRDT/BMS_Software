/*
 * bms.c
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#include "bms.h"

void timer_a0_init() //LTC use
{
    TA0CTL = 0x0000; //Clear any prior settings and put the timer in stop mode.
    TA0CTL |= 0x0100; //ACLK as source, ID = 0, TAIFG (count to 0) disabled since we're using capture/compare for both.
    TA0CCTL0 = 0x2000; // No capture, capture/compare input GND since we don't need it, capture/compare interrupt enabled.
    TA0CCTL1 = 0x2000;
    NVIC_EnableIRQ(TA0_0_IRQn);
    NVIC_EnableIRQ(TA0_N_IRQn);
    TA0CCTL0 |= BIT4;
    TA0CCTL1 |= BIT4;
    TA0CCR1 = STCVAD_CCR_DELAY; //Count up to here before starting a3 conversion.
    TA0CCR0 = STCVAD_CCR_DELAY + RDCV_CCR_DELAY; //Count up to here before trying to get adc results.
    TA0CTL |= 0x10; //Start TA0 in up mode.
}

void timer_a1_init() //Pack ADCs
{
    TA1CTL = 0x0000; //Clear any prior settings and put the timer in stop mode.
    TA1CTL |= 0x02C0; //SMCLK as source, ID = 8, TAIFG (count to 0) disabled since we're using capture/compare for both.
    TA1CCTL0 = 0x2000; // No capture, capture/compare input GND since we don't need it, capture/compare interrupt enabled.
    NVIC_EnableIRQ(TA1_0_IRQn);
    TA1CCTL0 |= BIT4;
    TA1CCR0 = PACK_MEAS_CCR_DELAY;
    TA1CTL |= 0x10; //Start TA1 in up mode.
}

void timer_a2_init() //Temp sensors
{
    TA2CTL = 0x0000;
    TA2CTL |= 0x0100;
    TA2CCTL0 |= 0x2000;
    TA2CCTL1 |= 0x2000;
    NVIC_EnableIRQ(TA2_0_IRQn);
    NVIC_EnableIRQ(TA2_N_IRQn);
    TA2CCTL0 |= BIT4;
    TA2CCTL1 |= BIT4;
    TA2CCR0 = STARTTEMP_CCR_DELAY + TEMP_MEAS_CCR_DELAY;
    TA2CCR1 = STARTTEMP_CCR_DELAY;
    TA2CTL |= 0x10; //Start TA2 in up mode.
}

void adc14_init()
{

//GPIO setup moved to main

//ADC setup: SAMPCON signal is sourced from sample timer, which is triggered by the ADC14SC bit.
//Sample and hold time for  registers 0-7 and 24-31 is set to 192 cycles.
//No clock division, sequence-of-channels mode,
P4SEL1 = BIT2;
P4SEL0 = (BIT2 | BIT5);
P6SEL0 = BIT0;
ADC14 -> CTL0 = ADC14_CTL0_PDIV__64 |
        ADC14_CTL0_SHP |
        ADC14_CTL0_SSEL__SMCLK |
        ADC14_CTL0_MSC |
        ADC14_CTL0_SHT0__192 |
        ADC14_CTL0_CONSEQ_1 |
        ADC14_CTL0_ON;
//Using 14 bit by default

//Use internal channels 0 through 2, mapped to I_PACK, V_CHECK_ARRAY, V_CHECK OUT
ADC14 -> MCTL[0] = ADC14_MCTLN_INCH_15; //External channel specifications with AVCC reference selected for all.
ADC14 -> MCTL[1] = ADC14_MCTLN_INCH_11;
ADC14 -> MCTL[2] = ADC14_MCTLN_INCH_8 | ADC14_MCTLN_EOS; //The third conversion is the end of the sequence.

NVIC_EnableIRQ(ADC14_IRQn); //Turn on interrupt for last conversion, first in the NVIC, then the ADC
ADC14 -> IER0 = ADC14_IER0_IE2;

ADC14 -> CTL0 |= ADC14_CTL0_ENC; //Enable conversion.
//This means config regs can't be written to without deasserting.
ADC14 -> CTL0 |= ADC14_CTL0_SC; // Start the first conversion
}


void timer_321_init()
{

}

void timer_322_init()
{

}

void clk_init() //Energia sets DCO to 48MHz and so should we.
{
//Also, to make timer use a little saner, let's plug REFOCLK into ACLK and turn it on.
//Using SMCLK at such a high frequency would mean we couldn't get 13ms delay in one cycle of timer A.
//Must first go to VCORE1; I learned this the hard (fault) way.
PCM -> CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_CPM__AM_LDO_VCORE1;
//No flash wait states at 48MHz creates unpredictable behavior too.
FLCTL -> BANK0_RDCTL |= FLCTL_BANK0_RDCTL_WAIT_2; //2 are recommended by TI for this frequency range, per slaa668.
CS -> KEY = CS_KEY_VAL;
CS -> CTL0 = CS_CTL0_DCORSEL_5;
CS -> CTL1 |= CS_CTL1_DIVS__4 | CS_CTL1_SELA__REFOCLK; //SMCLK should be 12MHz; REFOCLK should be 32768Hz
}

void tx_cvs()
{
    int i=0;
    for(i=0; i<12; i++)
       uart_tx(0, cell_regs[i]); //Send them packed; dealing with float conversion is too much trouble
}

