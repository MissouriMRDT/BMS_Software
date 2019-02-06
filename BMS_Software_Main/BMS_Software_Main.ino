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

RoveCommEthernetUdp RoveComm; //Extantiates a class

// Setup & Main Loop ////////////////////////////////////////////////////////////
//
void setup()
{
	Serial.begin(9600);
	Serial3.begin(9600);
	RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
	delay(ROVECOMM_DELAY);
	
	setInputPins();
	setOutputPins();
	setOutputStates();

	Serial.println("Setup Complete!");
} //end setup

void loop()
{
	uint16_t main_current;
	uint16_t cell_voltages[RC_BMSBOARD_VMEASmV_DATACOUNT];
	int pack_out_voltage;
  uint16_t batt_temp;
	uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT];
  static int num_loop = 0;
  static bool sw_ind_state = false;
	rovecomm_packet packet;

	getMainCurrent(main_current);
	getCellVoltage(cell_voltages);
	getOutVoltage(pack_out_voltage);
	getBattTemp(batt_temp);
	
	RoveComm.write(RC_BMSBOARD_MAINIMEASmA_HEADER, main_current);
  delay(ROVECOMM_DELAY);
	RoveComm.write(RC_BMSBOARD_VMEASmV_HEADER, cell_voltages);
  delay(ROVECOMM_DELAY);
	RoveComm.write(RC_BMSBOARD_TEMPMEASmDEGC_HEADER, batt_temp);
  delay(ROVECOMM_DELAY);

  checkOverCurrent(error_report);
  checkUnderVoltage(error_report);

  reactOverCurrent(error_report);
  reactUnderVoltage(error_report);
  //reactLowVoltage();
  reactOverTemp(batt_temp, overtemp_state);
  reactForgottenLogicSwitch(pack_out_voltage, forgotten_logic_switch, time_switch_forgotten, time_switch_reminder);

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
        //expand by adding more cases if more dataids are created
      } //end switch
    } //end if

    if(num_loop % BLINK_ON_LOOP == 0) //SW_IND led will blink while the code is looping
    {
      if(sw_ind_state == false)
      {
        digitalWrite(SW_IND_PIN, HIGH);
        sw_ind_state = true;
      }//end if
      if(sw_ind_state == true)
      {
        digitalWrite(SW_IND_PIN, LOW);
        sw_ind_state = false;
      }//end if
    }//end if

    num_loop++;
} //end loop