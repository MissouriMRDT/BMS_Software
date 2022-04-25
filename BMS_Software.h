// Battery Managment System (BMS) Software ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Main Header File
//
// Created for 2022 by: Sean Duda and Sanfan Liu
//
//
// Libraries /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#ifndef BMS_Software
#define BMS_Software


//RoveComm setup
#include "RoveComm.h"
//RoveCommEthernet RoveComm; //extantiates a class

// Pinmap ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Control Pins (output pins)
#define BUZZER_CTR_PIN			    37
#define FAN_CTR_PIN				    A15
#define SW_ERR_PIN				    A14
#define SW_IND_PIN				    A13
#define SER_TX_IND				    Serial1
#define PACK_GATE_CTR_PIN           36

//Sensor pins (input pins)
//cell sensors
#define C1_V_MEAS_PIN				A1
#define C2_V_MEAS_PIN				A2
#define C3_V_MEAS_PIN				A3
#define C4_V_MEAS_PIN				A4
#define C5_V_MEAS_PIN				A5
#define C6_V_MEAS_PIN				A6
#define C7_V_MEAS_PIN				A7
#define C8_V_MEAS_PIN				A8
//other system sensors
#define PACK_V_MEAS_PIN			    A10
#define PACK_I_MEAS_PIN			    A17
#define TEMP_degC_MEAS_PIN		    A12

const int CELL_MEAS_PINS[] = {C1_V_MEAS_PIN, C2_V_MEAS_PIN, C3_V_MEAS_PIN, C4_V_MEAS_PIN, C5_V_MEAS_PIN, C6_V_MEAS_PIN, C7_V_MEAS_PIN, C8_V_MEAS_PIN};

// Sensor Measurment: Constants and Calculations /////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tiva1294C RoveBoard Specs
#define VCC                   	    3300 //mV
//#define TIVA_ADC_MAX          4096 //bits
//#define TIVA_MIN              0 //bits

// Teensy 4.1 specs
  // Find at: https://www.pjrc.com/store/teensy41.html
  // find at: https://www.pjrc.com/store/teensy41_pins.html
#define SENSOR_SENSITIVITY   	    0.0066 //V  3.3V / 1024 levels = 0.0032

//value from past year
#define SENSOR_SCALE         	    0.5
#define SENSOR_BIAS          	    VCC * SENSOR_SCALE //Viout voltage when current is at 0A (aka quiescent output voltage)
                          						 // Noise is 2mV, meaning the smallest current that the device is able to resolve is 0.3A
// need to be tested for teensy
#define CURRENT_MAX           	    160000//mA; Current values must be sent over RoveComm as mA
#define CURRENT_MIN           	    -156800//mA
#define OVERCURRENT                 120000 //mA
#define CURRENT_ADC_MIN     	    0 //values
#define CURRENT_ADC_MAX     	    4096 //values

// Voltage Measurments
//cells
#define CELL_VOLTS_MIN              2400//ADC value  //Lowest cell voltage I expect to be able to measure.
#define CELL_VOLTS_MAX              4200 //ADC value //Highest possible cell voltage value we could measure.
#define CELL_V_ADC_MIN              651 //from analogread
#define CELL_V_ADC_MAX              922  //from analogread
#define CELL_COUNT	                8// number of modules
#define CELL_UNDERVOLTAGE           2400 //mV //If any cell voltage reaches this voltage, turn off rover and BMS suicide.

// package related
#define VOLTS_MIN                   0 //mV    //Lowest possible pack voltage value we could measure.
#define PACK_VOLTS_MAX        	    33600 //mV	//Highest possible pack voltage value we could measure.
#define PACK_UNDERVOLTAGE   	    19200 //mV	//If pack voltage reaches this voltage, turn off rover and BMS suicide.
#define PACK_LOWVOLTAGE     	    21200 //mV	//If pack voltage reaches this voltage, notify of low voltage.
#define PACK_V_ADC_MIN      	    0 //bits
#define PACK_V_ADC_MAX      	    4096 //bits
// TMP37 Temp Sensor Specs 
  //Find at: https://www.digikey.com/products/en?mpart=TMP37FT9Z&v=505
    //Voltage at 20 deg C is 500mV.
    //Scale Factor is 20mV/deg C.
#define TEMP_MIN          		    0 //mdeg C
#define TEMP_MAX          		    160000 //mdeg 	//Max temp than sensor can measure to and should correspond to adc value 4096.
#define TEMP_THRESHOLD      	    38000 //mdeg C  //About 100 degF
#define TEMP_ADC_MIN      		    0 //bits
#define TEMP_ADC_MAX      		    4096 //bits
#define NUM_TEMP_AVERAGE            10 //batt_temp will be average of this many measurements


// Delay Constants
#define ROVECOMM_DELAY      	    5 //msec		//Used after RoveComm.Write() to give data time to send.
#define DEBOUNCE_DELAY      	    10 //msec		//After an overcurrent or undervoltage is detected, delay by this and measure the pin again to confirm whether error is occurring.
#define RESTART_DELAY     		    5000 //msec		//Used after first overcurrent as delay before turning rover back on.
#define RECHECK_DELAY    		    10000 //msec	//Used to measure time since first overcurrent. If second overcurrent occurs within this delay time, BMS commits suicide.
#define LOGIC_SWITCH_REMINDER 	    60000 //msec 	//Every cycle of this period of time, the buzzer notifys someone that logic switch was forgotten.
#define IDLE_SHUTOFF_TIME   	    2400000 //msec or 40 minutes	//After this period of time passes, the BMS will commit suicide.
#define UPDATE_ON_LOOP     		    69 //loops		//Each time this number of loops passes, SW_IND will blink and LCD will update.
#define ROVECOMM_UPDATE_DELAY	    420 //ms

// Function Declarations ///////////////////////////////////////////////////////////

void setInputPins();


void setOutputPins();


void setOutputStates();


void getMainCurrent(float &main_current);


void getCellVoltage(float cell_voltage[RC_BMSBOARD_CELLV_MEAS_DATA_COUNT]);


void getOutVoltage(float &pack_out_voltage);


void getBattTemp(uint32_t &batt_temp);


void updateLCD(int32_t batt_temp, float cell_voltages[]);


void reactOverCurrent();


void reactUnderVoltage();


void reactOverTemp();


void reactForgottenLogicSwitch();


void reactEstopReleased();


void reactLowVoltage(float cell_voltage[RC_BMSBOARD_CELLV_MEAS_DATA_COUNT]);


void setEstop(uint8_t data);


void notifyEstop(); //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep


void notifyLogicSwitch(); //Buzzer sound: beeep beeep


void notifyEstopReleased(); //Buzzer sound: beep


void notifyReboot(); //Buzzer sound: beeeeeeeeeep beeep beeep


void notifyOverCurrent(); //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep


void notifyUnderVoltage(); //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep


void notifyLowVoltage(); //Buzzer Sound: beeep beeep beeep


void startScreen();


void stars();


void asterisks();


void movingRover();


#endif
