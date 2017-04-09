/*
 * ltc6803.c
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 */

#include "uart.h"
#include "ltc6803.h"
#include "spi.h"

//If you need to calculate the PEC for anything, use http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

const uint8_t CFGR_0[7] = {       0x18,   //CFGR0 - Level polling, 10 cell mode, stay in standby until a conversion
                                                        0x00,   //CFGR1 - No cell balancing
                                                        0x00,   //CFGR2 - Don't mask 1-4, no cell balancing
                                                        0xF0,   //CFGR3 - Mask the top four cells, 12, 11, 10, 9
                                                        0x00,   //CFGR4 - Don't care
                                                        0x00,   //CFGR5 - Don't care
                                                        0xE2 }; //PEC

const uint8_t CFGR_1[7] = {       0x19,   //CFGR0 - Level polling, 10 cell mode, stay in standby until a conversion
                                                        0x00,   //CFGR1 - No cell balancing
                                                        0x00,   //CFGR2 - Don't mask 1-4, no cell balancing
                                                        0xF0,   //CFGR3 - Mask the top four cells, 12, 11, 10, 9
                                                        0x00,   //CFGR4 - Don't care
                                                        0x00,   //CFGR5 - Don't care
                                                        0xCB }; //PEC


const uint8_t crc8_ccitt_table[256] = {
                                         0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
                                         0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
                                         0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
                                         0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
                                         0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
                                         0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
                                         0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
                                         0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
                                         0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
                                         0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
                                         0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
                                         0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
                                         0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
                                         0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
                                         0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
                                         0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
                                         0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
                                         0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
                                         0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
                                         0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
                                         0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
                                         0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
                                         0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
                                         0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
                                         0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
                                         0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
                                         0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
                                         0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
                                         0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
                                         0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
                                         0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
                                         0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
                                        };

uint8_t crc8_ccitt(uint8_t *data, int size)
{
    uint8_t crc=0x41;
    uint8_t *p = data;
    uint8_t *end = (uint8_t*)((int)p + size);
    while(p < end)
        crc = crc8_ccitt_table[crc ^ *(p++)];
    return crc;
}


void ltc6803_wrcfg(uint8_t cdc_mode)
{
  P10OUT &= ~BMS_CSBI;
  while(P10OUT & BMS_CSBI);
  __delay_cycles(100);
  int j=0;
  spi_tx(WRCFG);
  spi_tx(WRCFG_PEC);
  RESET_PEC;
  if(cdc_mode==0)
  {
      for(j=0; j<7; j++)
          spi_tx(CFGR_0[j]); //Check that cdc_mode is actually correct bit position
  }

  else
  {
      for(j=0; j<7; j++)
        spi_tx(CFGR_1[j]);
  }
  __delay_cycles(1000);
  P10OUT |= BMS_CSBI;
}

void ltc6803_stcvad()
{
  ltc6803_wrcfg(CDC1);
  __delay_cycles(500);
  P10OUT &= ~BMS_CSBI;
  while(P10OUT & BMS_CSBI);
  __delay_cycles(100); //96 worst case for 2us on LTC6803 timing diagram, 48MHz clock.
  spi_tx(STCVAD);
  spi_tx(STCVAD_PEC); //Remember to go back to CDC0 after receipt of data
}

void ltc6803_rdcv() //Assumes we've come in in CDC1, since this should only be called in the second timer interrupt after stcvad
{
  P10OUT |= BMS_CSBI;
  __delay_cycles(1000);
  int i=0, k=0;
  static uint8_t contCommsFaults;
  float vtgs[8];
  P10OUT &= ~BMS_CSBI;
  __delay_cycles(100);
  while(P10OUT & BMS_CSBI);
  spi_tx(RDCV);
  spi_tx(RDCV_PEC);
  for(i=0; i<16; i++)
    cell_regs[i] = spi_tx(RDCV);
  if(crc8_ccitt((&cell_regs[0]), 16))
  {
    contCommsFaults++;
    if(contCommsFaults == 5) //We got 5 PEC mismatches in a row, this most likely means something dire is wrong
    {
      __no_operation();//put some error handling here
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
  P10OUT |= BMS_CSBI;
  __delay_cycles(1000);
  P10OUT &= ~BMS_CSBI;
  while(P10OUT & BMS_CSBI);
  __delay_cycles(100);

  spi_tx(STCCLR);
  spi_tx(STCCLR_PEC);
  __delay_cycles(10000);
  P10OUT |= BMS_CSBI;
  __delay_cycles(1000);
  P10OUT &= ~BMS_CSBI;
  while(P10OUT & BMS_CSBI);
  __delay_cycles(100);

  //we just send cell_regs to the pb right now; the above conversion is to check for errors
  ltc6803_wrcfg(CDC0); //We're done with the measurement, so go back to standby until the next one.

}

void ltc6803_dagn()
{
    ltc6803_wrcfg(CDC1);
    __delay_cycles(500);
    P10OUT &= ~BMS_CSBI;
    while(P10OUT & BMS_CSBI);
    __delay_cycles(100); //96 worst case for 2us on LTC6803 timing diagram, 48MHz clock.
    spi_tx(DAGN);
    spi_tx(DAGN_PEC); //Remember to go back to CDC0 after receipt of data
}

void ltc6803_rddgnr()
{
    P10OUT |= BMS_CSBI;
    __delay_cycles(1000);
    uint8_t dr[3];
    int i=0;
    float test_vtg=0.0;
    P10OUT &= ~BMS_CSBI;
    __delay_cycles(100);
    while(P10OUT & BMS_CSBI);
    spi_tx(RDDGNR);
    spi_tx(RDDGNR_PEC);
    for(i=0; i<3; i++)
        dr[i] = spi_tx(RDDGNR);
    if(crc8_ccitt((&dr[0]), 3))
        {
            __no_operation(); //Probably don't need "real" error handling here since this is used with the debugger anyway
        }
    //Clear voltage regs so we trigger a fault on next read if there's no data
    __delay_cycles(1000);
    P10OUT |= BMS_CSBI;
    __delay_cycles(1000);
    P10OUT &= ~BMS_CSBI;
    while(P10OUT & BMS_CSBI);
    __delay_cycles(100);
    spi_tx(STCCLR);
    spi_tx(STCCLR_PEC);
    __delay_cycles(10000);
    P10OUT |= BMS_CSBI;
    __delay_cycles(1000);
    P10OUT &= ~BMS_CSBI;
    while(P10OUT & BMS_CSBI);
    __delay_cycles(100);
    ltc6803_wrcfg(CDC0); //We're done with the measurement, so go back to standby until the next one.
    test_vtg = ((dr[1] & 0x0F) << 8) | dr[0];
    test_vtg -= 512;
    test_vtg *= 1.5 * .001;
    __no_operation();
}

