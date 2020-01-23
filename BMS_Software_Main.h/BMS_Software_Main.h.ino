
#ifndef BMS_Software_Main
#define BMS_Software_Main

#include <Energia.h>
#include "RoveComm.h"
//RoveCommEthernetUdp RoveComm; 

// Pinmap // 

//Control Pins
#define BUZZER_CTL_PIN        PM_1    // change after new tiva
#define FAN_CTL_PIN           PK_0
#define PACK_OUT_CTL_PIN      PB_1
#define LOGIC_SWITCH_CTL_PIN  PA_4

//Indicator Pins
#define FAN_PWR_IND_PIN       PM_2    // change after new tiva
#define SW_IND_PIN            PB_0
#define SW_ERR_IND_PIN        PA_6

//Sensor Volts/Amps Reading Pins
#define PACK_I_MEAS_PIN       PE_1
#define PACK_V_MEAS_PIN       PE_2    //Measures voltage outputting to rover. Will read zero if PACK_OUT_CTL_PIN is LOW.
#define LOGIC_V_MEAS_PIN      PE_3    //Measures total voltage of pack continuously while logic switch is on. 
#define TEMP_degC_MEAS_PIN    PE_0

//Cell Voltage Pins
#define C1_V_MEAS_PIN         PE_5    //CELL 1 - GND
#define C2_V_MEAS_PIN         PE_4    //CELL 2 - CELL 1
#define C3_V_MEAS_PIN         PB_4    //CELL 3 - CELL 2
#define C4_V_MEAS_PIN         PB_5    //CELL 4 - CELL 3
#define C5_V_MEAS_PIN         PD_3    //CELL 5 - CELL 4
#define C6_V_MEAS_PIN         PD_2    //CELL 6 - CELL 5
#define C7_V_MEAS_PIN         PD_1    //CELL 7 - CELL 6
#define C8_V_MEAS_PIN         PD_0    //CELL 8 - CELL 7
const int CELL_MEAS_PINS[] = {LOGIC_V_MEAS_PIN,C1_V_MEAS_PIN,C2_V_MEAS_PIN,C3_V_MEAS_PIN,C4_V_MEAS_PIN,C5_V_MEAS_PIN,C6_V_MEAS_PIN,C7_V_MEAS_PIN,C8_V_MEAS_PIN};

//Sensor Measurement: Constants and Calculations//
//Tiva
#define VCC                   3300   //mV

//Current Sensor
#define SENSOR_SENSITIVITY    0.0066 //mV or mA
#define SENSOR_SCALE          0.5
#define SENSOR_BIAS           VCC * SENSOR_SCALE

#define CURRENT_MAX           200000 //mA
#define CURRENT_MIN          -196207 //mA
#define OVERCURRENT           100000 //mA
#define CURRENT_ADC_MIN       0      //bits
#define CURRENT_ADC_MAX       4096   //bits

//Voltage measurements
#define VOLTS_MIN             0      //mV
#define CELL_VOLTS_MIN        2400   //mV
#define PACK_VOLTS_MAX        33600  //mV
#define CELL_VOLTS_MAX        4200   //mV
#define PACK_UNDERVOLTAGE     21600  //mV
#define PACK_LOWVOLTAGE       25000  //mV
#define PACK_SAFETY_LOW       PACK_UNDERVOLTAGE - 4000 //mV
#define CELL_UNDERVOLTAGE     2650   //mV
#define PACK_EFFECTIVE_ZERO   5000   //mV
#define CELL_EFFECTIVE_ZERO   1000   //mV
#define PACK_V_ADC_MIN        0      //
#define PACK_V_ADC_MAX        4096
#define CELL_V_ADC_MIN        2320
#define CELL_V_ADC_MAX        3800
 
