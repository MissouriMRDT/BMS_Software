/*
 * spi.c
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */
#include "spi.h"


void spi_init() //To understand how this works, read chapters 22/23 of TI's slau356.
{
  UCB3CTL0 = UCSWRST; // Put eUSCI module in reset mode for configuration and clear everything else.
  UCB3CTL1 = 0x69; //Change on first clock edge; capture on second. SCLK inactive state is high; MSB first, 8-bit data, 3-pin synchronous master mode.
  UCB3CTL0 |= 0xC0; //Select SMCLK (sourced from DCOCLK on power-on, I don't believe any of the current code changes clock sources). Or it in so we stay in reset
  UCB3BR1 = 0x00; //Next two lines set clock divider. We set the DCO to 48MHz, which gives us a 24MHz SMCLK, which we then divide by 2 for 12MHz. We want 1MHz.
  UCB3BR0 = 0x0C;
  P10SEL0 = BMS_SCLK | BMS_MOSI | BMS_MISO;
  UCB3CTL0 &= ~UCSWRST; //Done with configuration; release from reset.
  spi_tx(0x00);
}

uint8_t spi_tx(uint8_t data)
{
  uint8_t rx_val;
  while(((EUSCI_B3 -> IFG) & EUSCI_B_IFG_TXIFG0) == 0); //If TXIFG0 isn't asserted, the tx shift register isn't full and we should wait.
  UCB3TXBUF = data;
  while(((EUSCI_B3 -> IFG) & EUSCI_B_IFG_RXIFG0) == 0); //If RXIFG0 isn't asserted, the rx shift register isn't copied over and we should wait.
  rx_val = UCB3RXBUF;
  return rx_val;
}
