// Battery Managment System (BMS) Software /////////////////////////////////////////
//
// Created for 2019 Valkyrie by: Jacob Lipina, jrlwd5
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

RoveCommEthernetUdp RoveComm; //??What does this do Van?

// Function Declarations ////////////////////////////////////////////////////////////
//
void getMainCurrent(RC_BMSBOARD_MAINIMEASmA_DATATYPE &main_current); //??func has & sign to pass value for main current back to program. Is it correct that it gets placed in "RC_BMSBOARD_BLAH main_current" var in loop?
void getCellVoltage(RC_BMSBOARD_VMEASmV_DATATYPE cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT]); //??How do the values in the array get back to the loop?
void getBattTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE &batt_temp);
void setInputPins();
void setOutputPins();
void setOutputStates();

// RoveComm DataIDs /////////////////////////////////////////////////////////////////
//
// Commands
const uint32_t SW_ESTOP							= 2000; // [delay] (ms, 0-off until reboot)
const uint32_t FAN_EN							= 2001; // [Fan1, Fan2, Fan3, Fan4] (0-Fan Disable, 1-Fan Enable)
const uint32_t BUZZER_EN						= 2002; //

// Telemetry
	// Pack Current
#define RC_BMSBOARD_MAINIMEASmA_DATAID         	= 00+_TYPE_TELEMETRY+_BMSBOARD_BOARDNUMBER
#define RC_BMSBOARD_MAINIMEASmA_DATATYPE       	= uint16_t  //main current output mA
#define RC_BMSBOARD_MAINIMEASmA_DATACOUNT      	= 1
#define RC_BMSBOARD_MAINIMEASmA_HEADER     		= RC_BMSBOARD_MAINIMEASmA_DATAID,RC_BMSBOARD_MAINIMEASmA_DATACOUNT        

	// Pack & Cell Voltages
#define RC_BMSBOARD_VMEASmV_DATAID             	= 01+_TYPE_TELEMETRY+_BMSBOARD_BOARDNUMBER
#define RC_BMSBOARD_VMEASmV_DATATYPE           	= uint16_t  //[Pack_Out, C1-G, C2-1, C3-2, C4-3, C5-4, C6-5, C7-6, C8-7]
#define RC_BMSBOARD_VMEASmV_DATACOUNT       	= 9 
#define RC_BMSBOARD_VMEASmV_HEADER       		= RC_BMSBOARD_VMEASmV_DATAID,RC_BMSBOARD_VMEASmV_DATACOUNT
#define RC_BMSBOARD_VMEASmV_PACKENTRY    		= 0     
#define RC_BMSBOARD_VMEASmV_C1GENTRY     		= 1
#define RC_BMSBOARD_VMEASmV_C21ENTRY     		= 2
#define RC_BMSBOARD_VMEASmV_C32ENTRY     		= 3
#define RC_BMSBOARD_VMEASmV_C43ENTRY     		= 4
#define RC_BMSBOARD_VMEASmV_C54ENTRY     		= 5
#define RC_BMSBOARD_VMEASmV_C65ENTRY     		= 6
#define RC_BMSBOARD_VMEASmV_C76ENTRY     		= 7
#define RC_BMSBOARD_VMEASmV_C87ENTRY     		= 8

	// Battery Temperature
#define RC_BMSBOARD_TEMPMEASmDEGC_DATAID       	= 02+_TYPE_TELEMETRY+_BMSBOARD_BOARDNUMBER
#define RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE     	= uint16_t  //Temperature Reading in mDeg Celcius
#define RC_BMSBOARD_TEMPMEASmDEGC_DATACOUNT   	= 1
#define RC_BMSBOARD_TEMPMEASmDEGC_HEADER       	= RC_BMSBOARD_TEMPMEASmDEGC_DATAID,RC_BMSBOARD_TEMPMEASmDEGC_DATACOUNT

const int ROVECOMM_DELAY						= 10; //??Is there a better delay const in the manifest?

// Pinmap //////////////////////////////////////////////////////////////////////////
//
// Control Pins
#define PACK_OUT_CTR 			= PD_1;
#define PACK_OUT_IND 			= PQ_2;
#define LOGIC_SWITCH_CTR		= PD_0;
#define BUZZER_CTR				= PN_2;
#define FAN_1_CTR				= PH_2;
#define FAN_2_CTR				= PH_3;
#define FAN_3_CTR				= PL_5;
#define FAN_4_CTR				= PL_4;
#define FAN_PWR_IND				= PF_1;
#define SW_IND					= PQ_3;
#define SW_ERR					= PP_3;

// Sensor Volts/Amps Readings Pins
#define PACK_I_MEAS				= PE_0;
#define PACK_V_MEAS				= PE_1;
#define LOG_V_MEAS				= PE_2;
#define TEMP_D_MEAS				= PM_3;
#define C1_GND_MEAS				= PK_3;
#define C2_C1_MEAS				= PK_2;
#define C3_C2_MEAS				= PK_1;
#define C4_C3_MEAS				= PK_0;
#define C5_C4_MEAS				= PB_5;
#define C6_C5_MEAS				= PB_4;
#define C7_C6_MEAS				= PD_5;
#define C8_C7_MEAS				= PD_4;

// Sensor Measurment: Constants and Calculations ////////////////////////////////////////////////
//
// Tiva1294C RoveBoard Specs
const float VCC                  = 3.3;       //volts
const float ADC_MAX            	 = 4096;      //bits
const float ADC_MIN            	 = 0;         //bits
const int IDLE_SHUTOFF_MINS		 = 30;

// ACS759ECB-200B-PFF-T Current Sensor Specs
	// Find at: https://www.digikey.com/products/en?keywords=%20620-1466-5-ND%20
const float SENSOR_SENSITIVITY   = 0.0066;    //volts/amp
const float SENSOR_SCALE         = 0.5;
const float SENSOR_BIAS          = VCC * SENSOR_SCALE;  //Viout voltage when current is at 0A (aka quiescent output voltage)
														// Noise is 2mV, meaning the smallest current that the device is able to resolve is 0.3A
const float CURRENT_MAX          = (VCC - SENSOR_BIAS - 0.33) / SENSOR_SENSITIVITY; // Amps
const float CURRENT_MIN          = -(SENSOR_BIAS - 0.33) / SENSOR_SENSITIVITY; // Amps
const float OVERCURRENT			 = 180; //TODO: This value should be lower, but where?

// Voltage Measurments
const float VOLTS_MIN            = 0;
const float PACK_VOLTS_MAX       = 33.6; //TODO: This num may change as we test using hardware
const float CELL_VOLTS_MAX 		 = 4.2;

// TMP37 Temp Sensor Specs 
	//Find at: https://www.digikey.com/products/en?mpart=TMP37FT9Z&v=505
		//Voltage at 20 deg C is 500mV
		//Scale Factor is 20mV/deg C
const float	 

// Minimum and Maximum Values: Constants

void setup()
{
	Serial3.begin(9600);
	RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
	delay(ROVECOMM_DELAY);
	
	setInputPins();
	setOutputPins();
	setOutputStates();
}

void loop()
{
	RC_BMSBOARD_MAINIMEASmA_DATATYPE main_current;
	RC_BMSBOARD_VMEASmV_DATATYPE cell_voltages[RC_BMSBOARD_VMEASmV_DATACOUNT]; //??"cell_voltages." Is the 's' there for a reason?
	RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE batt_temp;
	
	getMainCurrent(main_current);
	getCellVoltages(cell_voltages);
	getBattTemp(batt_temp);
	
	RoveComm.write(RC_BMSBOARD_MAINIMEASmA_HEADER, main_current);
	RoveComm.write(RC_BMSBOARD_VMEASmV_HEADER, cell_voltages); //??Van didnt use HEADER in github. Tell him
	RoveComm.write(RC_BMSBOARD_TEMPMEASmDEGC_HEADER, batt_temp);




}