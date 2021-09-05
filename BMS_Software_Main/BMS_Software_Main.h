
#ifndef BMS_Software_Main
const int  BMS_Software_Main

#i=nclude <Energia.h>
#include "RoveComm.h"
//RoveCommEthernetUdp RoveComm; 

// Pinmap // 

//Control Pins
const uint8_t BUZZER_CTRL_PIN       = PM_1;    
const uint8_t FAN_CTRL_PIN          = PK_0;
const uint8_t PACK_OUT_CTRL_PIN     = PB_1;
const uint8_t LOGIC_SWITCH_CTRL_PIN = PA_4;

//Indicator Pins
const uint8_t FAN_PWR_IND_PIN       = PM_2;
const uint8_t SW_IND_PIN            = PB_0;
const uint8_t SW_ERR_IND_PIN        = PA_6;

//Sensor Volts/Amps Reading Pins
const uint8_t PACK_I_MEAS_PIN       = PE_1;
const uint8_t PACK_V_MEAS_PIN       = PE_2;   //Measures voltage outputting to rover. Will read zero if PACK_OUT_CTL_PIN is LOW.
const uint8_t LOGIC_V_MEAS_PIN      = PE_3;  //Measures total voltage of pack continuously while logic switch is on. 
const uint8_t TEMP_degC_MEAS_PIN    = PE_0;

//Cell Voltage Pins
const uint8_t C1_V_MEAS_PIN         = PE_5;    //CELL 1 - GND
const uint8_t C2_V_MEAS_PIN         = PE_4;    //CELL 2 - CELL 1
const uint8_t C3_V_MEAS_PIN         = PB_4;   //CELL 3 - CELL 2
const uint8_t C4_V_MEAS_PIN         = PB_5;    //CELL 4 - CELL 3
const uint8_t C5_V_MEAS_PIN         = PD_3;    //CELL 5 - CELL 4
const uint8_t C6_V_MEAS_PIN         = PD_2;    //CELL 6 - CELL 5
const uint8_t C7_V_MEAS_PIN         = PD_1;    //CELL 7 - CELL 6
const uint8_t C8_V_MEAS_PIN         = PD_0;    //CELL 8 - CELL 7
const uint8_t CELL_MEAS_PINS[] = {C1_V_MEAS_PIN,C2_V_MEAS_PIN,C3_V_MEAS_PIN,C4_V_MEAS_PIN,C5_V_MEAS_PIN,C6_V_MEAS_PIN,C7_V_MEAS_PIN,C8_V_MEAS_PIN};

const int CURRENT_MAX       = 200000; //mA
const int CURRENT_MIN       = -196207; //mA
const int  OVERCURRENT      = 100000; //mA
const int  CURRENT_ADC_MIN  = 0;      //bits
const int  CURRENT_ADC_MAX  = 4096;   //bits

//Voltage measurements
const int  CELL_VOLTS_MIN   = 2400;   //mV
const int  CELL_VOLTS_MAX   = 4200;   //mV
const int  CELL_V_ADC_MIN   = 2450;   //bits
const int  CELL_V_ADC_MAX   = 4270;   //bits

const int  CELL_UNDERVOLTAGE    = 2650;   //mV
const int  CELL_SAFETY_LOW      = (PACK_SAFETY_LOW)/(RC_BMSBOARD_CELLV_MEAS_DATA_COUNT); //mV
const int  CELL_EFFECTIVE_ZERO  = 1000;   //mV

const float  PACK_UNDERVOLTAGE  = 26.00; //V
const float  PACK_LOWVOLTAGE    = 28.00;  //V

const float  PACK_SAFETY_LOW      = PACK_UNDERVOLTAGE - 4; //V
const float  PACK_EFFECTIVE_ZERO  = 5.00;   //V

//Temp 
const int  TEMP_MIN             = 0;     //mdeg C
const int  TEMP_MAX             = 140000; //mdeg
const int  TEMP_THRESHOLD       = 38000;  //mdeg C
const int  TEMP_ADC_MIN         = 0;      //bits 
const int  TEMP_ADC_MAX         = 4096;   //bits
const int  NUM_TEMP_AVERAGE     = 10;     //batt_temp will be average of this many measurements

//Delay Constants
const int  ROVECOMM_DELAY           = 5;        //msec    
const int  DEBOUNCE_DELAY           = 10;       //msec  
const int  RESTART_DELAY            = 5000;     //msec   
const int  RECHECK_DELAY            = 10000;    //msec  
const int  LOGIC_SWITCH_REMINDER    = 60000;  //msec 
const int  IDLE_SHUTOFF_TIME        = 2400000;  //msec 
const int  UPDATE_ON_LOOP           = 60;       //loops   
const int  ROVECOMM_UPDATE_DELAY    = 420;      //ms

//Error

void setInputPins();

void setOutputPins();

void setOutputStates();

float getTemperature();
float cells[RC_BMSBOARD_CELLV_MEAS_DATA_COUNT] getCellVoltages();
float getPackVoltage(float cells[RC_BMSBOARD_CELLV_MEAS_DATA_COUNT]);
float getCurrent();

#endif
