// Battery Managment System (BMS) Software /////////////////////////////////////////
	// Main cpp File
//
// Created for 2019 Valkyrie by: Jacob Lipina, jrlwd5
//
//
// Libraries ///////////////////////////////////////////////////////////////////////
//
#include "BMS_Software_Functions.h" 
#include "BMS_Software_Main.h"
#include <RoveComm.h>

/*
// Standard C
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
*/ //May not need standard C

RoveCommEthernetUdp RoveComm; //Extantiates a class

// Function Declarations ////////////////////////////////////////////////////////////
//
void getMainCurrent(RC_BMSBOARD_MAINIMEASmA_DATATYPE &main_current); //??func has & sign to pass value for main current back to program. Is it correct that it gets placed in "RC_BMSBOARD_BLAH main_current" var in loop?
void getCellVoltage(RC_BMSBOARD_VMEASmV_DATATYPE cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT]); //??How do the values in the array get back to the loop?
void getBattTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE &batt_temp);
void getOutVoltage(uint16_t &pack_out_voltage);
void setInputPins();
void setOutputPins();
void setOutputStates();
void setEstop(RC_BMSBOARD_SWESTOPs_DATATYPE data);
void setFans(RC_BMSBOARD_FANEN_DATATYPE data);
void notifyEstop();
void notifyReboot();
void notifyOverCurrent();
void notifyUnderVoltage();
void notifyLowVoltage();

const int ROVECOMM_DELAY		= 10; //??Is there a better delay const in the manifest?

// Pinmap //////////////////////////////////////////////////////////////////////////
//
// Control Pins
#define PACK_OUT_CTR 			 PD_1
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
#define LOGIC_V_MEAS			= PE_2;
#define TEMP_degC_MEAS			= PM_3;
#define C1_V_MEAS				= PK_3; //CELL 1 - GND
#define C2_V_MEAS				= PK_2; //CELL 2 - CELL 1
#define C3_V_MEAS				= PK_1; //CELL 3 - CELL 2
#define C4_V_MEAS				= PK_0; //CELL 4 - CELL 3
#define C5_V_MEAS				= PB_5; //CELL 5 - CELL 4
#define C6_V_MEAS				= PB_4; //CELL 6 - CELL 5
#define C7_V_MEAS				= PD_5; //CELL 7 - CELL 6
#define C8_V_MEAS				= PD_4; //CELL 8 - CELL 7

const int BLAHBLAHBLAHJACOBISDUMB[] = {PACK_V_MEAS, C1_V_MEAS}
// Sensor Measurment: Constants and Calculations ////////////////////////////////////////////////
//
// Tiva1294C RoveBoard Specs
const float VCC                 = 3.3;       //volts
const float ADC_MAX            	= 1024;      //bits
const float ADC_MIN            	= 0;         //bits
const int IDLE_SHUTOFF_MINS		= 30;

// ACS759ECB-200B-PFF-T Current Sensor Specs
	// Find at: https://www.digikey.com/products/en?keywords=%20620-1466-5-ND%20
const float SENSOR_SENSITIVITY  = 0.0066;    //volts/amp
const float SENSOR_SCALE        = 0.5;
const float SENSOR_BIAS         = VCC * SENSOR_SCALE;  //Viout voltage when current is at 0A (aka quiescent output voltage)
														// Noise is 2mV, meaning the smallest current that the device is able to resolve is 0.3A
const float CURRENT_MAX         = (VCC - SENSOR_BIAS - 0.33) / SENSOR_SENSITIVITY; // Amps
const float CURRENT_MIN         = -(SENSOR_BIAS - 0.33) / SENSOR_SENSITIVITY; // Amps
const float OVERCURRENT			= 180; //TODO: This value should be lower, but where?

// Voltage Measurments
const float VOLTS_MIN           = 0;
const float PACK_VOLTS_MAX      = 33.6; //TODO: This num may change as we test using hardware
const float CELL_VOLTS_MAX 		= 4.2;

// TMP37 Temp Sensor Specs 
	//Find at: https://www.digikey.com/products/en?mpart=TMP37FT9Z&v=505
		//Voltage at 20 deg C is 500mV
		//Scale Factor is 20mV/deg C
const float	TEMP_MIN = 0;//TODO: test temp conversions with a thermometer.
const float TEMP_MAX = 160;

// Minimum and Maximum Values: Constants


void setup()
{
	Serial.begin(9600);
	Serial3.begin(115600);
	RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
	delay(ROVECOMM_DELAY);
	
	setInputPins();
	setOutputPins();
	setOutputStates();

	Serial.println("Setup Complete!");
} //end setup

void loop()
{
	RC_BMSBOARD_MAINIMEASmA_DATATYPE main_current;
	RC_BMSBOARD_VMEASmV_DATATYPE cell_voltages[RC_BMSBOARD_VMEASmV_DATACOUNT]; //??"cell_voltages." Is the 's' there for a reason?
	RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE batt_temp;
	uint16_t pack_out_voltage;
	rovecomm_packet packet;

	getMainCurrent(main_current);
	getCellVoltages(cell_voltages);
	getOutVoltage(pack_out_voltage);
	getBattTemp(batt_temp);
	
	RoveComm.write(RC_BMSBOARD_MAINIMEASmA_HEADER, main_current);
	RoveComm.write(RC_BMSBOARD_VMEASmV_HEADER, cell_voltages); //??Van didnt use HEADER in github. Tell him, Change it myself
	RoveComm.write(RC_BMSBOARD_TEMPMEASmDEGC_HEADER, batt_temp);

	packet = RoveComm.read();
  	if(packet.data_id!=0)
  	{
      Serial.println(packet.data_id);
      Serial.println(packet.data_count);
      for(int i = 0; i<packet.data_count; i++)
      {
        Serial.print(packet.data[i]);
      } //end for
    
      switch(packet.data_id) //andrew needs to fix the type to int for the dataid
      {
        case RC_BMSBOARD_SWESTOPs_DATAID:
        {
          setEstop(packet.data[0]);
          break;
        }
        case RC_BMSBOARD_FANEN_DATAID:
        {
          setFans(packet.data[0]);
          break;
        }
      } //end switch
    } //end if


}