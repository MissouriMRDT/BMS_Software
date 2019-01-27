// Battery Managment System (BMS) Software /////////////////////////////////////////
//
// Created for 2019 Rover by: Jacob Lipina, jrlwd5
//
//
// Libraries ///////////////////////////////////////////////////////////////////////
//
#include <BMS_Software.h>
// 
// Standard C
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Energia
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// RoveWare
#include <RoveBoard.h>
#include <RoveEthernet.h>
#include <RoveComm.h>

// RoveComm DataIDs /////////////////////////////////////////////////////////////////
//
// Commands
const uint32_t SW_ESTOP						= 2000; // [delay] (ms, 0-off until reboot)
const uint32_t FAN_EN						  = 2001; // [Fan1, Fan2, Fan3, Fan4] (0-Fan Disable, 1-Fan Enable)
const uint32_t BUZZER_EN					= 2002; //

// Telemetry
const uint32_t I_MEAS					  	= 2100; // [Main] (mA)
const uint32_t V_MEAS					  	= 2101; // [Pack_Out, Logic, C1-G, C2-1, C3-2, C4-3, C5-4, C6-5, C7-6, C8-7] (mV)
const uint32_t TEMP_MEAS					= 2102; // [Temp] (mdegC)

const uint16_t NO_ROVECOMM_MESSAGE          = 0; // RED can toggle the bus by bool
const int ROVECOMM_DELAY 					= 10;

//Rovecomm :: RED packet :: data_id and data_value with number of data bytes size
uint16_t data_id       = 0;
size_t   data_size     = 0; //Change once Rovecomm 2 is finished
uint8_t  data_value    = 0;

// Pinmap //////////////////////////////////////////////////////////////////////////
//
// Control Pins
#define PACK_OUT_CTR 					= PD_1;
#define PACK_OUT_IND 					= PQ_2;
#define LOGIC_SWITCH_CTR			= PD_0;
#define BUZZER_CTR						= PN_2;
#define FAN_1_CTR					  	= PH_2;
#define FAN_2_CTR				  		= PH_3;
#define FAN_3_CTR				   		= PL_5;
#define FAN_4_CTR					  	= PL_4;
#define FAN_PWR_IND						= PF_1;
#define SW_IND					   		= PQ_3;
#define SW_ERR					   		= PP_3;

// Sensor Volts/Amps Readings Pins
#define PACK_I_MEAS						= PE_0;
#define PACK_V_MEAS						= PE_1;
#define LOG_V_MEAS						= PE_2;
#define TEMP_D_MEAS						= PM_3;
#define C1_GND_MEAS						= PK_3;
#define C2_C1_MEAS						= PK_2;
#define C3_C2_MEAS						= PK_1;
#define C4_C3_MEAS						= PK_0;
#define C5_C4_MEAS						= PB_5;
#define C6_C5_MEAS						= PB_4;
#define C7_C6_MEAS						= PD_5;
#define C8_C7_MEAS						= PD_4;

// Constants and Calculations for Sensor Measurments ////////////////////////////////////////////////
//
// Tiva1294C RoveBoard Specs
const float VCC                 = 3.3;       //volts
const float ADC_MAX             = 4096;      //bits
const float ADC_MIN             = 0;         //bits
const int IDLE_SHUTOFF_MINS		  = 30;

// ACS759ECB-200B-PFF-T Current Sensor Specs
	// Find at: https://www.digikey.com/products/en?keywords=%20620-1466-5-ND%20
const float SENSOR_SENSITIVITY   = 0.0066;    //volts/amp
const float SENSOR_SCALE         = 0.5;
const float SENSOR_BIAS          = VCC * SENSOR_SCALE;  //Viout voltage when current is at 0A (aka quiescent output voltage)
	// Noise is 2mV, meaning the smallest current that the device is able to resolve is 0.3A
const float CURRENT_MAX          = (VCC - SENSOR_BIAS - 0.33) / SENSOR_SENSITIVITY; // Amps
const float CURRENT_MIN          = -(SENSOR_BIAS - 0.33) / SENSOR_SENSITIVITY; // Amps
const float OVERCURRENT			     = 180; //TODO: This value should be lower, but where?

// Voltage Measurments
const float VOLTS_MIN            = 0;
const float PACK_VOLTS_MAX       = 33.6; //This num may change as we test using hardware
const float CELL_VOLTS_MAX 		   = 4.2;

const int DEBOUNCE_DELAY         = 10;

// DS18B20 Temp Sensor Specs 
	//Find at: https://www.digikey.com/product-detail/en/maxim-integrated/DS18B20/DS18B20-ND/420071
const float 

