// Battery Managment System (BMS) Software /////////////////////////////////////////
	// Functions Header File
//
// Created for 2019 Valkyrie by: Jacob Lipina, jrlwd5
//
//
// Libraries ///////////////////////////////////////////////////////////////////////
//
#ifndef BMS_Software_Main
#define BMS_Software_Main
#include "RoveComm.h"
//
// Function Declarations ///////////////////////////////////////////////////////////

void setInputPins();


void setOutputPins();


void setOutputStates();


void getMainCurrent(uint16_t &main_current);


void getCellVoltage(uint16_t cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT]); //??How is cell_voltage[] passed back to cpp?


void getOutVoltage(int &pack_out_voltage);


void getBattTemp(uint16_t &batt_temp);


bool singleDebounceCurrent(int bouncing_pin, int overcurrent_threshold); // Checks the pin for bouncing voltages to avoid false positives


bool singleDebounceVoltage(int bouncing_pin, int undervoltage_threshold, int volts_max, int volts_safety_low);


void checkOverCurrent(uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT]);


void checkUnderVoltage(uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT]);


void reactOverCurrent(uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT], int &num_overcurrent, float &time_of_overcurrent);


void reactUnderVoltage(uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT]);


void reactOverTemp(uint16_t batt_temp, bool &overtemp_state);


void reactForgottenLogicSwitch(int pack_out_voltage, bool &forgotten_logic_switch, int &time_switch_forgotten, int &time_switch_reminder);


void setEstop(uint8_t data);


void setFans(uint8_t data); //make sure command turning fans on does not get overridden by the temp being too low.


void notifyEstop(); //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep


void notifyLogicSwitch(); //Buzzer sound: beeep beeep


void notifyReboot(); //Buzzer sound: beeeeeeeeeep beeep beeep


void notifyOverCurrent(); //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep


void notifyUnderVoltage(); //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep


void notifyLowVoltage(); //Buzzer Sound: beeep beeep beeep


#endif