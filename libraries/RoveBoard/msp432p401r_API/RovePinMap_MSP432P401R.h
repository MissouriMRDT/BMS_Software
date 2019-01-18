/*
 * RovePinMap_MSP432P401R.h
 *
 *  Created on: Oct 16, 2017
 *      Author: drue
 */

#ifndef ROVEBOARD_MSP432P401R_API_ROVEPINMAP_MSP432P401R_H_
#define ROVEBOARD_MSP432P401R_API_ROVEPINMAP_MSP432P401R_H_

#include "msp432Ware/inc/msp.h"
#include "msp432Ware/driverlib/driverlib.h"

const uint8_t P1_0 = 4;
const uint8_t P1_1 = 5;
const uint8_t P1_2 = 6;
const uint8_t P1_3 = 7;
const uint8_t P1_4 = 8;
const uint8_t P1_5 = 9;
const uint8_t P1_6 = 10;
const uint8_t P1_7 = 11;

const uint8_t P2_0 = 16;
const uint8_t P2_1 = 17;
const uint8_t P2_2 = 18;
const uint8_t P2_3 = 19;
const uint8_t P2_4 = 20;
const uint8_t P2_5 = 21;
const uint8_t P2_6 = 22;
const uint8_t P2_7 = 23;

const uint8_t P3_0 = 32;
const uint8_t P3_1 = 33;
const uint8_t P3_2 = 34;
const uint8_t P3_3 = 35;
const uint8_t P3_4 = 36;
const uint8_t P3_5 = 37;
const uint8_t P3_6 = 38;
const uint8_t P3_7 = 39;

const uint8_t P4_0 = 56;
const uint8_t P4_1 = 57;
const uint8_t P4_2 = 58;
const uint8_t P4_3 = 59;
const uint8_t P4_4 = 60;
const uint8_t P4_5 = 61;
const uint8_t P4_6 = 62;
const uint8_t P4_7 = 63;

const uint8_t P5_0 = 64;
const uint8_t P5_1 = 65;
const uint8_t P5_2 = 66;
const uint8_t P5_3 = 67;
const uint8_t P5_4 = 68;
const uint8_t P5_5 = 69;
const uint8_t P5_6 = 70;
const uint8_t P5_7 = 71;

const uint8_t P6_0 = 54;
const uint8_t P6_1 = 55;
const uint8_t P6_2 = 76;
const uint8_t P6_3 = 77;
const uint8_t P6_4 = 78;
const uint8_t P6_5 = 79;
const uint8_t P6_6 = 80;
const uint8_t P6_7 = 81;

const uint8_t P7_0 = 88;
const uint8_t P7_1 = 89;
const uint8_t P7_2 = 90;
const uint8_t P7_3 = 91;
const uint8_t P7_4 = 26;
const uint8_t P7_5 = 27;
const uint8_t P7_6 = 28;
const uint8_t P7_7 = 29;

const uint8_t P8_0 = 30;
const uint8_t P8_1 = 31;
const uint8_t P8_2 = 46;
const uint8_t P8_3 = 47;
const uint8_t P8_4 = 48;
const uint8_t P8_5 = 49;
const uint8_t P8_6 = 50;
const uint8_t P8_7 = 51;

const uint8_t P9_0 = 52;
const uint8_t P9_1 = 53;
const uint8_t P9_2 = 74;
const uint8_t P9_3 = 75;
const uint8_t P9_4 = 96;
const uint8_t P9_5 = 97;
const uint8_t P9_6 = 98;
const uint8_t P9_7 = 99;

const uint8_t P10_0 = 100;
const uint8_t P10_1 = 1;
const uint8_t P10_2 = 2;
const uint8_t P10_3 = 3;
const uint8_t P10_4 = 24;
const uint8_t P10_5 = 25;

static const uint16_t NotAPin = 0xFFFE;

static const uint16_t pinToPinPort[] =
{
  NotAPin,                     //dummy
  GPIO_PORT_P10,               //1           P10.1       digital, UCB3CLK
  GPIO_PORT_P10,               //2           P10.2       digital, UCB3SIMO, UCB3SDA
  GPIO_PORT_P10,               //3           P10.3       digital, UCB3SOMI, UCB3SCL
  GPIO_PORT_P1,                //4           P1.0        digital, UCA0STE
  GPIO_PORT_P1,                //5           P1.1        digital, UCA0CLK
  GPIO_PORT_P1,                //6           P1.2        digital, UCA0RXD, UCA0SOMI
  GPIO_PORT_P1,                //7           P1.3        digital, UCA0TXD, UCA0SIMO
  GPIO_PORT_P1,                //8           P1.4        digital, UCB0STE
  GPIO_PORT_P1,                //9           P1.5        digital, UCB0CLK
  GPIO_PORT_P1,                //10          P1.6        digital, UCB0SIMO, UCB0SDA
  GPIO_PORT_P1,                //11          P1.7        digital, UCB0SOMI, UCB0SCL
  NotAPin,                     //12          N/A         VCORE
  NotAPin,                     //13          N/A         DVCC1
  NotAPin,                     //14          N/A         VSW
  NotAPin,                     //15          N/A         DVSS1
  GPIO_PORT_P2,                //16          P2.0        digital, PM_UCA1STE
  GPIO_PORT_P2,                //17          P2.1        digital, PM_UCA1CLK
  GPIO_PORT_P2,                //18          P2.2        digital, PM_UCA1RXD, PM_UCA1SOMI
  GPIO_PORT_P2,                //19          P2.3        digital, PM_UCA1TXD, PM_UCA1SIMO
  GPIO_PORT_P2,                //20          P2.4        digital, PM_TA0.1
  GPIO_PORT_P2,                //21          P2.5        digital, PM_TA0.2
  GPIO_PORT_P2,                //22          P2.6        digital, PM_TA0.3
  GPIO_PORT_P2,                //23          P2.7        digital, PM_TA0.4
  GPIO_PORT_P10,               //24          P10.4       digital, TA3.0, C0.7
  GPIO_PORT_P10,               //25          P10.5       digital, TA3.1, C0.6
  GPIO_PORT_P7,                //26          P7.4        digital, PM_TA1.4, C0.5
  GPIO_PORT_P7,                //27          P7.5        digital, PM_TA1.3, C0.4
  GPIO_PORT_P7,                //28          P7.6        digital, PM_TA1.2, C0.3
  GPIO_PORT_P7,                //29          P7.7        digital, PM_TA1.1, C0.2
  GPIO_PORT_P8,                //30          P8.0        digital, UCB3STE, TA1.0, C0.1
  GPIO_PORT_P8,                //31          P8.1        digital, UCB3CLK, TA2.0, C0.0
  GPIO_PORT_P3,                //32          P3.0        digital, PM_UCA2STE
  GPIO_PORT_P3,                //33          P3.1        digital, PM_UCA2CLK
  GPIO_PORT_P3,                //34          P3.2        digital, PM_UCA2RXD, PM_UCA2SOMI
  GPIO_PORT_P3,                //35          P3.3        digital, PM_UCA2TXD, PM_UCA2SIMO
  GPIO_PORT_P3,                //36          P3.4        digital, PM_UCB2STE
  GPIO_PORT_P3,                //37          P3.5        digital, PM_UCB2CLK
  GPIO_PORT_P3,                //38          P3.6        digital, PM_UCB2SIMO, PM_UCB2SDA
  GPIO_PORT_P3,                //39          P3.7        digital, PM_UCB2SOMI, PM_UCB2SCL
  NotAPin,                     //40          N/A         AVSS3
  NotAPin,                     //41          PJ.0        digital, LFXIN  Technically can be used, but it's a crystal CLK port so way too important
  NotAPin,                     //42          PJ.1        digital, LFXOUT
  NotAPin,                     //43          N/A         AVSS1
  NotAPin,                     //44          N/A         DCOR
  NotAPin,                     //45          N/A         AVCC1
  GPIO_PORT_P8,                //46          P8.2        digital, TA3.2, A23
  GPIO_PORT_P8,                //47          P8.3        digital, TA3CLK, A22
  GPIO_PORT_P8,                //48          P8.4        digital, A21
  GPIO_PORT_P8,                //49          P8.5        digital, A20
  GPIO_PORT_P8,                //50          P8.6        digital, A19
  GPIO_PORT_P8,                //51          P8.7        digital, A18
  GPIO_PORT_P9,                //52          P9.0        digital, A17
  GPIO_PORT_P9,                //53          P9.1        digital, A16
  GPIO_PORT_P6,                //54          P6.0        digital, A15
  GPIO_PORT_P6,                //55          P6.1        digital, A14
  GPIO_PORT_P4,                //56          P4.0        digital, A13
  GPIO_PORT_P4,                //57          P4.1        digital, A12
  GPIO_PORT_P4,                //58          P4.2        digital, ACLK, TA2CLK, A11
  GPIO_PORT_P4,                //59          P4.3        digital, MCLK, RTCCLK, A10
  GPIO_PORT_P4,                //60          P4.4        digital, HSMCLK, SVMHOUT, A9
  GPIO_PORT_P4,                //61          P4.5        digital, A8
  GPIO_PORT_P4,                //62          P4.6        digital, A7
  GPIO_PORT_P4,                //63          P4.7        digital, A6
  GPIO_PORT_P5,                //64          P5.0        digital, A5
  GPIO_PORT_P5,                //65          P5.1        digital, A4
  GPIO_PORT_P5,                //66          P5.2        digital, A3
  GPIO_PORT_P5,                //67          P5.3        digital, A2
  GPIO_PORT_P5,                //68          P5.4        digital, A1
  GPIO_PORT_P5,                //69          P5.5        digital, A0
  GPIO_PORT_P5,                //70          P5.6        digital, TA2.1, VREF+, VeREF+, C1.7
  GPIO_PORT_P5,                //71          P5.7        digital, TA2.2, VREF-, VeREF-, C1.6
  NotAPin,                     //72          N/A         DVSS2
  NotAPin,                     //73          N/A         DVCC2
  GPIO_PORT_P9,                //74          P9.2        digital, TA3.3
  GPIO_PORT_P9,                //75          P9.3        digital, TA3.4
  GPIO_PORT_P6,                //76          P6.2        digital, UCB1STE, C1.5
  GPIO_PORT_P6,                //77          P6.3        digital, UCB1CLK, C1.4
  GPIO_PORT_P6,                //78          P6.4        digital, UCB1SIMO, UCB1SDA, C1.3
  GPIO_PORT_P6,                //79          P6.5        digital, UCB1SOMI, UCB1SCL, C1.2
  GPIO_PORT_P6,                //80          P6.6        digital, TA2.3, UCB3SIMO, UCB3SDA, C1.1
  GPIO_PORT_P6,                //81          P6.7        digital, TA2.4, UCB3SOMI, UCB3SCL, C1.0
  NotAPin,                     //82          N/A         DVSS3
  NotAPin,                     //83          N/A         !reset, NMI
  NotAPin,                     //84          N/A         AVSS2
  NotAPin,                     //85          PJ.2        digital, HFXOUT
  NotAPin,                     //86          PJ.3        digital, HFXIN
  NotAPin,                     //87          N/A         AVCC2
  GPIO_PORT_P7,                //88          P7.0        digital, PM_SMCLK, PM_DMAE0
  GPIO_PORT_P7,                //89          P7.1        digital, PM_C0OUT, PM_TA0CLK
  GPIO_PORT_P7,                //90          P7.2        digital, PM_C1OUT, PM_TA1CLK
  GPIO_PORT_P7,                //91          P7.3        digital, PM_TA0.0
  NotAPin,                     //92          PJ.4        digital, TDI
  NotAPin,                     //93          PJ.5        digital, TD0, SWO
  NotAPin,                     //94          N/A         SWDIOTMS
  NotAPin,                     //95          N/A         SWCLKTCK
  GPIO_PORT_P9,                //96          P9.4        digital, UCA3STE
  GPIO_PORT_P9,                //97          P9.5        digital, UCA3CLK
  GPIO_PORT_P9,                //98          P9.6        digital, UCA3RXD, UCA3SOMI
  GPIO_PORT_P9,                //99          P9.7        digital, UCA3TXD, UCA3SIMO
  GPIO_PORT_P10               //100         P10.0       digital, UCB3STE
};


static const uint16_t pinToPinMask[] =
{
  NotAPin,                  //dummy
  GPIO_PIN1,                //1           P10.1       digital, UCB3CLK
  GPIO_PIN2,                //2           P10.2       digital, UCB3SIMO, UCB3SDA
  GPIO_PIN3,                //3           P10.3       digital, UCB3SOMI, UCB3SCL
  GPIO_PIN0,                //4           P1.0        digital, UCA0STE
  GPIO_PIN1,                //5           P1.1        digital, UCA0CLK
  GPIO_PIN2,                //6           P1.2        digital, UCA0RXD, UCA0SOMI
  GPIO_PIN3,                //7           P1.3        digital, UCA0TXD, UCA0SIMO
  GPIO_PIN4,                //8           P1.4        digital, UCB0STE
  GPIO_PIN5,                //9           P1.5        digital, UCB0CLK
  GPIO_PIN6,                //10          P1.6        digital, UCB0SIMO, UCB0SDA
  GPIO_PIN7,                //11          P1.7        digital, UCB0SOMI, UCB0SCL
  NotAPin,                  //12          N/A         VCORE
  NotAPin,                  //13          N/A         DVCC1
  NotAPin,                  //14          N/A         VSW
  NotAPin,                  //15          N/A         DVSS1
  GPIO_PIN0,                //16          P2.0        digital, PM_UCA1STE
  GPIO_PIN1,                //17          P2.1        digital, PM_UCA1CLK
  GPIO_PIN2,                //18          P2.2        digital, PM_UCA1RXD, PM_UCA1SOMI
  GPIO_PIN3,                //19          P2.3        digital, PM_UCA1TXD, PM_UCA1SIMO
  GPIO_PIN4,                //20          P2.4        digital, PM_TA0.1
  GPIO_PIN5,                //21          P2.5        digital, PM_TA0.2
  GPIO_PIN6,                //22          P2.6        digital, PM_TA0.3
  GPIO_PIN7,                //23          P2.7        digital, PM_TA0.4
  GPIO_PIN4,                //24          P10.4       digital, TA3.0, C0.7
  GPIO_PIN5,                //25          P10.5       digital, TA3.1, C0.6
  GPIO_PIN4,                //26          P7.4        digital, PM_TA1.4, C0.5
  GPIO_PIN5,                //27          P7.5        digital, PM_TA1.3, C0.4
  GPIO_PIN6,                //28          P7.6        digital, PM_TA1.2, C0.3
  GPIO_PIN7,                //29          P7.7        digital, PM_TA1.1, C0.2
  GPIO_PIN0,                //30          P8.0        digital, UCB3STE, TA1.0, C0.1
  GPIO_PIN1,                //31          P8.1        digital, UCB3CLK, TA2.0, C0.0
  GPIO_PIN0,                //32          P3.0        digital, PM_UCA2STE
  GPIO_PIN1,                //33          P3.1        digital, PM_UCA2CLK
  GPIO_PIN2,                //34          P3.2        digital, PM_UCA2RXD, PM_UCA2SOMI
  GPIO_PIN3,                //35          P3.3        digital, PM_UCA2TXD, PM_UCA2SIMO
  GPIO_PIN4,                //36          P3.4        digital, PM_UCB2STE
  GPIO_PIN5,                //37          P3.5        digital, PM_UCB2CLK
  GPIO_PIN6,                //38          P3.6        digital, PM_UCB2SIMO, PM_UCB2SDA
  GPIO_PIN7,                //39          P3.7        digital, PM_UCB2SOMI, PM_UCB2SCL
  NotAPin,                  //40          N/A         AVSS3
  NotAPin,                  //41          PJ.0        digital, LFXIN  Technically can be used, but it's a crystal CLK port so way too important
  NotAPin,                  //42          PJ.1        digital, LFXOUT
  NotAPin,                  //43          N/A         AVSS1
  NotAPin,                  //44          N/A         DCOR
  NotAPin,                  //45          N/A         AVCC1
  GPIO_PIN2,                //46          P8.2        digital, TA3.2, A23
  GPIO_PIN3,                //47          P8.3        digital, TA3CLK, A22
  GPIO_PIN4,                //48          P8.4        digital, A21
  GPIO_PIN5,                //49          P8.5        digital, A20
  GPIO_PIN6,                //50          P8.6        digital, A19
  GPIO_PIN7,                //51          P8.7        digital, A18
  GPIO_PIN0,                //52          P9.0        digital, A17
  GPIO_PIN1,                //53          P9.1        digital, A16
  GPIO_PIN0,                //54          P6.0        digital, A15
  GPIO_PIN1,                //55          P6.1        digital, A14
  GPIO_PIN0,                //56          P4.0        digital, A13
  GPIO_PIN1,                //57          P4.1        digital, A12
  GPIO_PIN2,                //58          P4.2        digital, ACLK, TA2CLK, A11
  GPIO_PIN3,                //59          P4.3        digital, MCLK, RTCCLK, A10
  GPIO_PIN4,                //60          P4.4        digital, HSMCLK, SVMHOUT, A9
  GPIO_PIN5,                //61          P4.5        digital, A8
  GPIO_PIN6,                //62          P4.6        digital, A7
  GPIO_PIN7,                //63          P4.7        digital, A6
  GPIO_PIN0,                //64          P5.0        digital, A5
  GPIO_PIN1,                //65          P5.1        digital, A4
  GPIO_PIN2,                //66          P5.2        digital, A3
  GPIO_PIN3,                //67          P5.3        digital, A2
  GPIO_PIN4,                //68          P5.4        digital, A1
  GPIO_PIN5,                //69          P5.5        digital, A0
  GPIO_PIN6,                //70          P5.6        digital, TA2.1, VREF+, VeREF+, C1.7
  GPIO_PIN7,                //71          P5.7        digital, TA2.2, VREF-, VeREF-, C1.6
  NotAPin,                  //72          N/A         DVSS2
  NotAPin,                  //73          N/A         DVCC2
  GPIO_PIN2,                //74          P9.2        digital, TA3.3
  GPIO_PIN3,                //75          P9.3        digital, TA3.4
  GPIO_PIN2,                //76          P6.2        digital, UCB1STE, C1.5
  GPIO_PIN3,                //77          P6.3        digital, UCB1CLK, C1.4
  GPIO_PIN4,                //78          P6.4        digital, UCB1SIMO, UCB1SDA, C1.3
  GPIO_PIN5,                //79          P6.5        digital, UCB1SOMI, UCB1SCL, C1.2
  GPIO_PIN6,                //80          P6.6        digital, TA2.3, UCB3SIMO, UCB3SDA, C1.1
  GPIO_PIN7,                //81          P6.7        digital, TA2.4, UCB3SOMI, UCB3SCL, C1.0
  NotAPin,                  //82          N/A         DVSS3
  NotAPin,                  //83          N/A         !reset, NMI
  NotAPin,                  //84          N/A         AVSS2
  NotAPin,                  //85          PJ.2        digital, HFXOUT
  NotAPin,                  //86          PJ.3        digital, HFXIN
  NotAPin,                  //87          N/A         AVCC2
  GPIO_PIN0,                //88          P7.0        digital, PM_SMCLK, PM_DMAE0
  GPIO_PIN1,                //89          P7.1        digital, PM_C0OUT, PM_TA0CLK
  GPIO_PIN2,                //90          P7.2        digital, PM_C1OUT, PM_TA1CLK
  GPIO_PIN3,                //91          P7.3        digital, PM_TA0.0
  NotAPin,                  //92          PJ.4        digital, TDI
  NotAPin,                  //93          PJ.5        digital, TD0, SWO
  NotAPin,                  //94          N/A         SWDIOTMS
  NotAPin,                  //95          N/A         SWCLKTCK
  GPIO_PIN4,                //96          P9.4        digital, UCA3STE
  GPIO_PIN5,                //97          P9.5        digital, UCA3CLK
  GPIO_PIN6,                //98          P9.6        digital, UCA3RXD, UCA3SOMI
  GPIO_PIN7,                //99          P9.7        digital, UCA3TXD, UCA3SIMO
  GPIO_PIN0,               //100         P10.0       digital, UCB3STE/
};


#endif /* ROVEBOARD_MSP432P401R_API_ROVEPINMAP_MSP432P401R_H_ */
