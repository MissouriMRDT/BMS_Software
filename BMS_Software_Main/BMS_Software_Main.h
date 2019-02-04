// Battery Managment System (BMS) Software /////////////////////////////////////////
	// Main Header File
//
// Created for 2019 Valkyrie by: Jacob Lipina, jrlwd5
//
//
// Libraries ///////////////////////////////////////////////////////////////////////
//
#ifndef BMS_Software_Functions
#define BMS_Software_Functions

#include <Energia.h>
#include "RoveComm.h"
//
//
// Pinmap //////////////////////////////////////////////////////////////////////////
//
// Control Pins
#define PACK_OUT_CTR 			PD_1
#define PACK_OUT_IND 			PQ_2
#define LOGIC_SWITCH_CTR		PD_0
#define BUZZER_CTR				PN_2
#define FAN_1_CTR				PH_2
#define FAN_2_CTR				PH_3
#define FAN_3_CTR				PL_5
#define FAN_4_CTR				PL_4
#define FAN_PWR_IND				PF_1
#define SW_IND					PQ_3
#define SW_ERR					PP_3

// Sensor Volts/Amps Readings Pins
#define PACK_I_MEAS				PE_0
#define PACK_V_MEAS				PE_1 //Measures voltage outputting to rover. Will read zero if PACK_OUT_CTR is LOW.
#define LOGIC_V_MEAS			PE_2 //Measures total voltage of pack continuously while logic switch is on. 
#define TEMP_degC_MEAS			PM_3
#define C1_V_MEAS				PK_3 //CELL 1 - GND
#define C2_V_MEAS				PK_2 //CELL 2 - CELL 1
#define C3_V_MEAS				PK_1 //CELL 3 - CELL 2
#define C4_V_MEAS				PK_0 //CELL 4 - CELL 3
#define C5_V_MEAS				PB_5 //CELL 5 - CELL 4
#define C6_V_MEAS				PB_4 //CELL 6 - CELL 5
#define C7_V_MEAS				PD_5 //CELL 7 - CELL 6
#define C8_V_MEAS				PD_4 //CELL 8 - CELL 7

const int CELL_MEAS_PINS[] = {LOGIC_V_MEAS, C1_V_MEAS, C2_V_MEAS, C3_V_MEAS, 
 							  C4_V_MEAS, C5_V_MEAS, C6_V_MEAS, C7_V_MEAS, C8_V_MEAS};

// Sensor Measurment: Constants and Calculations ////////////////////////////////////////////////
//
// Tiva1294C RoveBoard Specs
#define VCC                 	3300 //mV
#define ADC_MAX            		1024 //bits
#define ADC_MIN            		0 //bits

// ACS759ECB-200B-PFF-T Current Sensor Specs
	// Find at: https://www.digikey.com/products/en?keywords=%20620-1466-5-ND%20
#define SENSOR_SENSITIVITY  	0.0066 //mV/mA
#define SENSOR_SCALE        	0.5
#define SENSOR_BIAS         	VCC * SENSOR_SCALE //Viout voltage when current is at 0A (aka quiescent output voltage)
													// Noise is 2mV, meaning the smallest current that the device is able to resolve is 0.3A
#define CURRENT_MAX         	(VCC - SENSOR_BIAS - 330) / SENSOR_SENSITIVITY //mA; Current values must be sent over RoveComm as mA
#define CURRENT_MIN         	-(SENSOR_BIAS - 330) / SENSOR_SENSITIVITY //mA
#define OVERCURRENT				50000 //mA //TODO: This value should be lower, but where?

// Voltage Measurments
#define VOLTS_MIN           	0 //mV
#define PACK_VOLTS_MAX      	33600 //mV //TODO: This num may change as we test using hardware
#define CELL_VOLTS_MAX 			4200 //mV
#define PACK_UNDERVOLTAGE		21600 //mV
#define PACK_LOWVOLTAGE			25000 //mV
#define PACK_SAFETY_LOW			PACK_UNDERVOLTAGE - 4000 //mV
#define CELL_UNDERVOLTAGE		2700 //mV
#define CELL_SAFETY_LOW			CELL_UNDERVOLTAGE - 1000 //mV
#define LOGIC_LOW_VOLTS			VOLTS_MIN + 5000 //mV

// TMP37 Temp Sensor Specs 
	//Find at: https://www.digikey.com/products/en?mpart=TMP37FT9Z&v=505
		//Voltage at 20 deg C is 500mV
		//Scale Factor is 20mV/deg C
#define	TEMP_MIN   				0 //TODO: test temp conversions with a thermometer.
#define TEMP_MAX   				160000 //mdeg C
#define TEMP_THRESHOLD			38000 //mdeg C  //About 100 degF

// Delay Constants
#define ROVECOMM_DELAY			10 //msec
#define DEBOUNCE_DELAY			10 //msec
#define RESTART_DELAY			5000 //msec
#define RECHECK_DELAY			40000 //msec
#define LOGIC_SWITCH_REMINDER	60000 //msec
#define IDLE_SHUTOFF_MINS		2400000 //msec or 40 minutes

#endif