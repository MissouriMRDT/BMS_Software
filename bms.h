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
#include <gpio.h>
#include <pcm.h>
#include <stdbool.h>
#include <stdint.h>
#include "spi.h"
#include "uart.h"
#include "ds18b20.h"

//Delays. "CCR" indicates we're loading something in a compare reg for a timer.
#define REBOOT_DELAY        5000
#define IDLE_DELAY          3600000 //the rover sits idle for 1 hour before turning the BMS logic off.
#define REBOOT_TRY_COUNT    3
#define DEBOUNCE_DELAY      10 //10ms; give to a delay function
#define STCVAD_CCR_DELAY    0xE000 //This goes in TA0CCR1. Should translate to a half second for ACLK at 32768Hz with ID = 0 sourcing timer A.
//#define RDCV_CCR_DELAY      0x290 //20ms -- 13ms for adc conversion to complete, plus 7ms for safety per Jesse Cureton.
                                    // This is an artifact of the LTC6803 design. Leaving it in case someone goes back.
#define PACK_MEAS_CCR_DELAY 0x927C //25ms with clock divider 8 and 12MHz from SMCLK -- recommended delay for current measurement, guess I might as well throw voltages in here too
#define STARTTEMP_CCR_DELAY 0x9A6E
#define TEMP_MEAS_CCR_DELAY 0x6590
#define SERIAL_DELAY        10
#define OVFS_CELL_START     40 //Number of TA1 overflows before cell conversion start

#define V_CELL_CHNL     ADC14_MCTLN_INCH_14
#define V_OUT_CHNL      ADC14_MCTLN_INCH_3
#define V_ARRAY_CHNL    ADC14_MCTLN_INCH_4
#define I_PACK_CHNL     ADC14_MCTLN_INCH_5

//Pins for GPIO conf etc. Note the port they go on.

//Port 1
#define FAN_CTRL_1      GPIO_PIN6
#define FAN_CTRL_2      GPIO_PIN7

//Port 2
#define TEMP_1          GPIO_PIN5

//Port 3
#define PACK_GATE       GPIO_PIN0
#define SER_RX_PB       GPIO_PIN2
#define SER_TX_PB       GPIO_PIN3
#define LOGIC_SWITCH    GPIO_PIN5
#define FAN_CTRL_3       GPIO_PIN6
#define FAN_CTRL_4       GPIO_PIN7

//Port 4
#define ADC_CELL_A0     GPIO_PIN0
#define ADC_CELL_A1     GPIO_PIN1
#define ADC_CELL_A2     GPIO_PIN2
#define ADC_CELL_EN     GPIO_PIN4

//Port 5
#define PACK_I_MEAS     GPIO_PIN0
#define V_CHECK_ARRAY   GPIO_PIN1
#define V_CHECK_OUT     GPIO_PIN2
#define BUZZER          GPIO_PIN7

//Port 6
#define ADC_CELL_VOUT   GPIO_PIN1

//Port 8
#define LED_SER_IN      GPIO_PIN4
#define LED_RCK         GPIO_PIN5

//Port 9
#define GAUGE_ON        GPIO_PIN0
#define LED_SRCK        GPIO_PIN1
#define SER_RX_IND      GPIO_PIN6
#define SER_TX_IND      GPIO_PIN7


// MSP432 RoveBoard Specs
#define VCC             3.3       //volts
#define ADC_MAX         16384      //bits -- 14 bit; Remember this is gonna change if you change ADC resolution
#define ADC_MIN         0         //bits
#define LOOP_DELAY      10      //ms
#define IDLE_SHUTOFF_MINS   59

//ACS_759 IC Sensor Specs
#define SENSOR_SENSITIVITY  0.0066    //volts/amp. This is dependent on the [somenumber]B suffix on the current sensor -- 50B -> 50A max and associated scale from datasheet
#define SENSOR_SCALE        0.5
//#define SENSOR_BIAS         VCC*SENSOR_SCALE //V. for now- determine empirically later
#define SENSOR_BIAS         1.655
#define AMPS_MAX            (VCC - SENSOR_BIAS-0.33)/SENSOR_SENSITIVITY //amps
#define AMPS_MIN             -(SENSOR_BIAS-0.33)/SENSOR_SENSITIVITY      //amps
#define AMP_OVERCURRENT     180 //amps
#define RDIV_BIAS           9.26 //check if used later

//Voltage reading
#define VOLTS_MAX               5*11
#define VOLTS_MIN               0
#define BATTERY_LOW             2.7*8  //V  warning low voltage
#define BATTERY_LOW_CRIT        2.5*8  //V  low voltage threshold for shutoff to protect pack

uint16_t adc14_out[4];

unsigned char ind_tx_buf[130]; //"Cell voltages xx.xx xx.xx xx.xx xx.xx xx.xx xx.xx xx.xx xx.xx" 61
                                //"Pack voltage xx.xx" 18
                                //"Pack current: xx.xx" 20
                                //"Pack temp: xxx.xx" 17 incl. null
                                //Total len: 129

//Easy way to tx adc results byte by byte
union txable_float {
    float f;
    unsigned char ch[4];
};

float pack_vtg_out;

union txable_float ow_temp_reading; //Rover side
union txable_float pack_vtg_array; //Pack temperature
union txable_float pack_i; //Result of ADC on PACK_I_MEAS

int j, h, mins, current_cell, conv_counts, conv_counts_last;

union txable_float cell_vtgs[8], cell_vtgs_last[8];

bool manual_fans, cell_v_writelock;

void rtc_init();

void timer_a0_init(); //Timer A0 config for LTC interaction

void timer_a1_init(); //Need shorter interval for pack readings

void timer_a2_init();

void adc14_init();

void clk_init();

void tx_cvs();


#endif /* BMS_H_ */
