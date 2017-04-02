/*
 * uart.c
 *
 *  Created on: Feb 18, 2017
 *      Author: Joseph Hall
 *
 *
 *
 */
#include "uart.h"
#include "msp.h"

void uart_init()
{
    //Two targets -- power and indicator boards, on A2 and A3 respectively.
    //LSB first, UART/asynchronous mode, no parity, 8-bit data, 1 stop bit.
    //SMCLK at 12MHz used to generate 115200 baudrate
    EUSCI_A2 -> CTLW0 = EUSCI_A_CTLW0_SWRST;
    EUSCI_A3 -> CTLW0 = EUSCI_A_CTLW0_SWRST;
    EUSCI_A2 -> CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
    EUSCI_A3 -> CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
    EUSCI_A2 -> BRW = 0x0006; //Straight from datasheet for 12MHz, 115200 baud
    EUSCI_A3 -> BRW = 0x0006;
    EUSCI_A2 -> MCTLW = 0x2081;
    EUSCI_A3 -> MCTLW = 0x2081;
    P3SEL0 = PB_RX | PB_TX;
    P9SEL0 = IND_RX | IND_TX;
    EUSCI_A2 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
    EUSCI_A3 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
    NVIC_EnableIRQ(EUSCIA2_IRQn);
    EUSCI_A2 -> IE = 0x01; //RX interrupt enabled. Might add TX later so no time is spent waiting, but for now this is fine.
}

void uart_tx(uint8_t target, uint8_t data)
{
    if(target == TARGET_PB)
    {
        while(!((EUSCI_A2 -> IFG) & EUSCI_A_IFG_TXIFG)); //Wait for TX buffer to be ready
        EUSCI_A2 -> TXBUF = data;
    }
    else if(target == TARGET_IND)
    {
        while(!((EUSCI_A3 -> IFG) & EUSCI_A_IFG_TXIFG));
        EUSCI_A3 -> TXBUF = data;
    }
}
