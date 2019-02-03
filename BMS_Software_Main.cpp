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


void setup()
{
	Serial.begin(9600);
	Serial3.begin(115200);
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
	int pack_out_voltage;
	rovecomm_packet packet;

	getMainCurrent(main_current);
	getCellVoltages(cell_voltages);
	getOutVoltage(pack_out_voltage);
	getBattTemp(batt_temp);
	
	RoveComm.write(RC_BMSBOARD_MAINIMEASmA_HEADER, main_current);
	RoveComm.write(RC_BMSBOARD_VMEASmV_HEADER, cell_voltages);
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


} //end loop