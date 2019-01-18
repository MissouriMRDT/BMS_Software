/*
 * SysInit.cpp
 *
 *  Created on: Oct 15, 2017
 *      Author: drue
 */

#include <stdint.h>
#include "../Clocking/Clocking_MSP432P401R.h"
#include "../msp432Ware/inc/msp.h"

void initSystem()
{

  // Enable FPU
  SCB->CPACR |= ((3UL << 10 * 2) |                       // Set CP10 Full Access
                (3UL << 11 * 2));                       // Set CP11 Full Access

  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;            // Halt the WDT

  SYSCTL->SRAM_BANKEN = SYSCTL_SRAM_BANKEN_BNK7_EN;      // Enable all SRAM banks

  // Switches LDO VCORE0 to LDO VCORE1; mandatory for 48 MHz setting
  while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
  PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
  while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));

  // 1 flash wait states (BANK0 VCORE1 max is 16 MHz, BANK1 VCORE1 max is 32 MHz)
  FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~FLCTL_BANK0_RDCTL_WAIT_MASK) | FLCTL_BANK0_RDCTL_WAIT_1;
  FLCTL->BANK1_RDCTL = (FLCTL->BANK1_RDCTL & ~FLCTL_BANK1_RDCTL_WAIT_MASK) | FLCTL_BANK1_RDCTL_WAIT_1;

  // Set Flash Bank read buffering
  FLCTL->BANK0_RDCTL = FLCTL->BANK0_RDCTL | (FLCTL_BANK0_RDCTL_BUFD | FLCTL_BANK0_RDCTL_BUFI);
  FLCTL->BANK1_RDCTL = FLCTL->BANK1_RDCTL | (FLCTL_BANK1_RDCTL_BUFD | FLCTL_BANK1_RDCTL_BUFI);

  CS->KEY |= CS_KEY_VAL;                                  // Unlock CS module for register access
  initSystemClocks();
  BITBAND_PERI(CS->KEY, CS_KEY_KEY_OFS) = 1;

}
