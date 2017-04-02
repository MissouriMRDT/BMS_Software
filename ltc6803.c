/*
 * ltc6803.c
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#include "uart.h"
#include "ltc6803.h"
#include "spi.h"

const uint8_t CFGR[6] = {       0x18,   //CFGR0 - Level polling, 10 cell mode, stay in standby until a conversion
                                                        0x00,   //CFGR1 - No cell balancing
                                                        0x00,   //CFGR2 - Don't mask 1-4, no cell balancing
                                                        0xF0,   //CFGR3 - Mask the top four cells, 12, 11, 10, 9
                                                        0x00,   //CFGR4 - Don't care
                                                        0x00  };//CFGR5 - Don't care


void ltc6803_pec(uint8_t val)
{
        uint8_t curBit = 0; //TODO: See about moving some of this computation to a lookup table in ROM, this is an inefficient way to do CRC8-CCITT
        int i=0;                                                                    //8-bit value with the current bit in its LSB
        for(i = 0; i < 8; i++)                                                        //Loop through every bit in the byte
        {
            curBit   = (val >> (7-i)) & 0x01;                                              //Isolate the the (i-1)th bit of val
            pecIN    = curBit ^ ((pecVAL >> 7) & 0x01);                                    //Initialize pecIN to (0b0000000X where X is curBit) XOR MSB of PEC
            pecIN   |= ((pecVAL & 0x01) ^ (pecIN & 0x01)) << 1;                            //Magic (IN1 = PEC[0] XOR IN0)
            pecIN   |= (((pecVAL >> 1) & 0x01) ^ (pecIN & 0x01)) << 2;                     //More magic (IN2 = PEC[1] XOR IN0)
            pecVAL   = ((pecVAL << 1) & ~0x07) | pecIN;                                    //Set the current PEC value
        }
}


void ltc6803_wrcfg(uint8_t cdc_mode)
{
  P5OUT &= ~BMS_CSBI;
  while(P5OUT & BMS_CSBI);
  __delay_cycles(100);
  int i=0;
  spi_tx(WRCFG);
  spi_tx(WRCFG_PEC);
  RESET_PEC;
  for(i=0; i<6; i++)
  {
    if(i==0) spi_tx(CFGR[i] | cdc_mode); //Check that cdc_mode is actually correct bit position
    else spi_tx(CFGR[i]);
    if(i==0) ltc6803_pec(CFGR[i] | cdc_mode);
    else ltc6803_pec(CFGR[i]);
  }
  spi_tx(pecVAL);
  __delay_cycles(1000);
  P5OUT |= BMS_CSBI;
}

void ltc6803_stcvad()
{
  ltc6803_wrcfg(CDC1);
  __delay_cycles(500);
  P5OUT &= ~BMS_CSBI;
  while(P5OUT & BMS_CSBI);
  __delay_cycles(100); //96 worst case for 2us on LTC6803 timing diagram, 48MHz clock.
  spi_tx(STCVAD);
  spi_tx(STCVAD_PEC); //Remember to go back to CDC0 after receipt of data
}

void ltc6803_rdcv() //Assumes we've come in in CDC1, since this should only be called in the second timer interrupt after stcvad
{
  P5OUT |= BMS_CSBI;
  __delay_cycles(1000);
  uint8_t rxd_PEC=0, unused_cvr=0;
  int i=0, j=0, k=0;
  static uint8_t contCommsFaults;
  float vtgs[8];
  P5OUT &= ~BMS_CSBI;
  __delay_cycles(100);
  while(P5OUT & BMS_CSBI);
  spi_tx(RDCV);
  spi_tx(RDCV_PEC);
  RESET_PEC;
  for(i=0; i<12; i++)
  {
    cell_regs[i] = spi_tx(RDCV);
    ltc6803_pec(cell_regs[i]);
  }
  for(j=0; j<3; j++)
  {
      unused_cvr = spi_tx(0x00); //Toss top 3 cell regs, since they're for cells 9 and 10, which are unconnected.
      ltc6803_pec(unused_cvr);
  }
  rxd_PEC = spi_tx(0x00);
  if(rxd_PEC!=pecVAL)
  {
    contCommsFaults++;
    if(contCommsFaults == 5) //We got 5 PEC mismatches in a row, this most likely means something dire is wrong
    {
      //put some error handling here
    }
  }
  else contCommsFaults = 0;


  //The CVR0x registers, as read in from SPI, are 8 bits, but the actual voltages are 12-bit floats
  // spread out across multiple registers. Consult the LTC6803 datasheet (table 8, p. 23)
  //
  //The "split" occurs at the most significant nybble, then at the least significant, then most again, etc.
  //You could probably put this in a cute little for loop, but I don't care yet.
  vtgs[0] = cell_regs[0] | ((cell_regs[1] & 0x0F) << 8); //Cell 1
  vtgs[1] = ((cell_regs[1] & 0xF0) >> 4) | (cell_regs[2] << 4); //Cell 2
  vtgs[2] = cell_regs[3] | ((cell_regs[4] & 0x0F) << 8); //Cell 3
  vtgs[3] = ((cell_regs[4] & 0xF0) >> 4) | (cell_regs[5] << 4); //Cell 4
  vtgs[4] = cell_regs[6] | ((cell_regs[7] & 0x0F) << 8); //Cell 5
  vtgs[5] = ((cell_regs[7] & 0xF0) >> 4) | (cell_regs[8] << 4);//Cell 6
  vtgs[6] = cell_regs[9] | ((cell_regs[10] & 0x0F) << 8); //Cell 7
  vtgs[7] = ((cell_regs[10] & 0xF0) >> 4) | (cell_regs[11] << 4);//Cell 8

  //These still aren't normal voltages after putting them in floats; you need to do some extra processing.
  for(k = 0; k < 8; k++)
  {
    vtgs[k] -= 512;
    vtgs[k] *= 1.5 * .001; //From LTC6803 datasheet p. 14
  }
  //Clear voltage regs so we trigger a fault on next read if there's no data
  __delay_cycles(1000);
  P5OUT |= BMS_CSBI;
  __delay_cycles(1000);
  P5OUT &= ~BMS_CSBI;
  while(P5OUT &BMS_CSBI);
  __delay_cycles(100);

  spi_tx(STCCLR);
  spi_tx(STCCLR_PEC);
  __delay_cycles(10000);
  P5OUT |= BMS_CSBI;
  __delay_cycles(1000);
  P5OUT &= ~BMS_CSBI;
  while(P5OUT & BMS_CSBI);
  __delay_cycles(100);

  //we just send cell_regs to the pb right now; the above conversion is to check for errors
  ltc6803_wrcfg(CDC0); //We're done with the measurement, so go back to standby until the next one.

}

void ltc6803_dagn()
{
    ltc6803_wrcfg(CDC1);
    __delay_cycles(500);
    P5OUT &= ~BMS_CSBI;
    while(P5OUT & BMS_CSBI);
    __delay_cycles(100); //96 worst case for 2us on LTC6803 timing diagram, 48MHz clock.
    spi_tx(DAGN);
    spi_tx(DAGN_PEC); //Remember to go back to CDC0 after receipt of data
}

void ltc6803_rddgnr()
{
    P5OUT |= BMS_CSBI;
    __delay_cycles(1000);
    uint8_t dr0=0, dr1=0, dr_pec, testval;
    float test_vtg=0.0;
    P5OUT &= ~BMS_CSBI;
    __delay_cycles(100);
    while(P5OUT & BMS_CSBI);
    spi_tx(RDDGNR);
    spi_tx(RDDGNR_PEC);
    RESET_PEC;
    dr0 = spi_tx(RDDGNR);
    ltc6803_pec(dr0);
    dr1 = spi_tx(RDDGNR);
    ltc6803_pec(dr1);
    dr_pec = spi_tx(RDDGNR);
    testval = spi_tx(RDDGNR);
    //Clear voltage regs so we trigger a fault on next read if there's no data
    __delay_cycles(1000);
    P5OUT |= BMS_CSBI;
    __delay_cycles(1000);
    P5OUT &= ~BMS_CSBI;
    while(P5OUT & BMS_CSBI);
    __delay_cycles(100);
    spi_tx(STCCLR);
    spi_tx(STCCLR_PEC);
    __delay_cycles(10000);
    P5OUT |= BMS_CSBI;
    __delay_cycles(1000);
    P5OUT &= ~BMS_CSBI;
    while(P5OUT & BMS_CSBI);
    __delay_cycles(100);
    ltc6803_wrcfg(CDC0); //We're done with the measurement, so go back to standby until the next one.
    test_vtg = ((dr1 & 0x0F) << 8) | dr0;
    test_vtg -= 512;
    test_vtg *= 1.5 * .001;
    __no_operation();
}

