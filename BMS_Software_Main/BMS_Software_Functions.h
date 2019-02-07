// Battery Managment System (BMS) Software /////////////////////////////////////////
	// Functions Header File
//
// Created for 2019 Valkyrie by: Jacob Lipina, jrlwd5
//
//
// Libraries ///////////////////////////////////////////////////////////////////////
//
#ifndef BMS_Software_Functions
#define BMS_Software_Functions

#include "RoveComm.h"

RoveCommEthernetUdp RoveComm; //Extantiates a class
// Function Declarations ///////////////////////////////////////////////////////////


void setInputPins();


void setOutputPins();


void setOutputStates();


void getMainCurrent(uint16_t &main_current);


void getCellVoltage(uint16_t cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT], uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT]);


void getOutVoltage(int &pack_out_voltage);


void getBattTemp(uint16_t &batt_temp);


void reactOverCurrent(uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT]);


void reactUnderVoltage(uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT]);


void reactOverTemp();


void reactForgottenLogicSwitch();


void setEstop(uint8_t data);


void notifyEstop(); //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep


void notifyLogicSwitch(); //Buzzer sound: beeep beeep


void notifyReboot(); //Buzzer sound: beeeeeeeeeep beeep beeep


void notifyOverCurrent(); //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep


void notifyUnderVoltage(); //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep


void notifyLowVoltage(); //Buzzer Sound: beeep beeep beeep


#endif