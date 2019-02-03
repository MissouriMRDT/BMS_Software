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


void getMainCurrent(RC_BMSBOARD_MAINIMEASmA_DATATYPE &main_current);


void getCellVoltage(RC_BMSBOARD_VMEASmV_DATATYPE cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT]); //??How is cell_voltage[] passed back to cpp?


void getOutVoltage(int &pack_out_voltage);


void getBattTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE &batt_temp);


bool singleDebounce(int bouncing_pin, int max_amps_threshold); // Checks the pin for bouncing voltages to avoid false positives


void setEstop(RC_BMSBOARD_SWESTOPs_DATATYPE data);


void setFans(RC_BMSBOARD_FANEN_DATATYPE data); //make sure command turning fans on does not get overridden by the temp being too low.


void notifyEstop(); //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep


void notifyReboot(); //Buzzer sound: beeeeeeeeeep beeep beeep


void notifyOverCurrent(); //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep


void notifyUnderVoltage(); //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep


void notifyLowVoltage(); //Buzzer Sound: beeep beeep beeep


#endif