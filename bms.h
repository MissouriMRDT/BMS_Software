/*
 * bms.h
 *
 *  Created on: Feb 9, 2017
 *      Author: Joseph Hall
 *      Pulling everything together for the main loop.
 *      Also a catch-all for functions and defines that don't fit elsewhere.
 */

#ifndef BMS_H_
#define BMS_H_
#include "msp.h"
#include "msp432p401r.h"
#include "spi.h"
#include "uart.h"
#include "ltc6803.h"
#include "ds18b20.h"

//Delays. "CCR" indicates we're loading something in a compare reg for a timer.
#define REBOOT_DELAY        5000
#define IDLE_DELAY          3600000 //the rover sits idle for 1 hour before turning the BMS logic off.
#define REBOOT_TRY_COUNT    3
#define DEBOUNCE_DELAY      10 //10ms, do we have a milliseconds utility in CCS? May need to recreate.
#define STCVAD_CCR_DELAY    0xE000 //This goes in TA0CCR1. Should translate to a half second for ACLK at 32768Hz with ID = 0 sourcing timer A.
#define RDCV_CCR_DELAY      0x290 //20ms -- 13ms for adc conversion to complete, plus 7ms for safety per Jesse Cureton.
                                    //This plus the STCVAD_DELAY should be in TA0CCR0 for the above settings.
#define PACK_MEAS_CCR_DELAY 0xAFC8 //15ms with clock divider 8 and 12MHz from SMCLK -- recommended delay for current measurement, guess I might as well throw voltages in here too
#define STARTTEMP_CCR_DELAY 0x9A6E
#define TEMP_MEAS_CCR_DELAY 0x6590
#define SERIAL_DELAY        10

#define V_OUT_CHNL      ADC14_MCTLN_INCH_3
#define V_ARRAY_CHNL    ADC14_MCTLN_INCH_4
#define I_PACK_CHNL     ADC14_MCTLN_INCH_5

//Pins for GPIO conf etc. Note the port they go on.

//Port 1
#define FAN_CTRL_1      BIT6
#define FAN_CTRL_2      BIT7

//Port 2
#define TEMP_1          BIT5

//Port 3
#define PACK_GATE       BIT0
#define SER_RX_PB       BIT2
#define SER_TX_PB       BIT3
#define LOGIC_SWITCH    BIT5
#define FAN_CTRL_3       BIT6
#define FAN_CTRL_4       BIT7

//Port 5
#define PACK_I_MEAS     BIT0
#define V_CHECK_ARRAY   BIT1
#define V_CHECK_OUT     BIT2
#define BUZZER          BIT7

//Port 8
#define LED_SER_IN      BIT4
#define LED_RCK         BIT5

//Port 9
#define GAUGE_ON        BIT0
#define LED_SRCK        BIT1
#define SER_RX_IND      BIT6
#define SER_TX_IND      BIT7

//Port 10
#define BMS_CSBI        BIT0
#define BMS_SCLK        BIT1
#define BMS_MOSI        BIT2
#define BMS_MISO        BIT3

//#define LED1            78 Dunno what this is, let's worry about it later

// MSP432 RoveBoard Specs
#define VCC             3.3       //volts
#define ADC_MAX         16384      //bits -- 14 bit; Remember this is gonna change if you change ADC resolution
#define ADC_MIN         0         //bits
#define LOOP_DELAY      10;       //ms

//ACS_759 IC Sensor Specs
#define SENSOR_SENSITIVITY  0.0066    //volts/amp
#define SENSOR_SCALE        0.5
#define SENSOR_BIAS         VCC*SENSOR_SCALE //V. for now- determine empirically later
#define AMPS_MAX            (VCC - SENSOR_BIAS-0.33)/SENSOR_SENSITIVITY //amps
#define AMPS_MIN             -(SENSOR_BIAS-0.33)/SENSOR_SENSITIVITY      //amps
#define AMP_OVERCURRENT     180 //amps

//Voltage reading
#define VOLTS_MAX               5*11
#define VOLTS_MIN               0
#define BATTERY_LOW             2.7*8  //V  warning low voltage
#define BATTERY_LOW_CRIT        2.5*8  //V  low voltage threshold for shutoff to protect pack

uint16_t adc14_out[3];

//Easy way to tx adc results byte by byte
union txable_float {
    float f;
    unsigned char ch[4];
};

float pack_vtg_out;

union txable_float ow_temp_reading; //Rover side
union txable_float pack_vtg_array; //Pack side
union txable_float pack_i; //Result of ADC on PACK_I_MEAS

uint8_t pb_command;

int j;

void timer_a0_init(); //Timer A0 config for LTC interaction

void timer_a1_init(); //Need shorter interval for pack readings

void timer_a2_init();

void adc14_init();

void clk_init();

void tx_cvs();



#endif /* BMS_H_ */
