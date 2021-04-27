#include <Energia.h>
#include "RoveComm.h"
//sets up RoveComm

//LED control pins
const uint8_t SW_IND_PIN=              PF_1;
const uint8_t SW_ERR_IND_PIN=          PM_3;
const uint8_t V_OUT_IND_PIN=           PH_2;
const uint8_t FANS_IND_PIN=            PH_3;

//Sensor pins
const uint8_t TEMP_SENSE_PIN=          PD_1;
const uint8_t V_OUT_SENSE_PIN=         PD_0;
const uint8_t PACK_I_SENSE_PIN=        PN_4;
const uint8_t LOGIC_POWER_SENSE_PIN=   PK_7;

//Cell voltage pins
const uint8_t CELL1_VOLTAGE_PIN=       PK_3;
const uint8_t CELL2_VOLTAGE_PIN=       PK_2;
const uint8_t CELL3_VOLTAGE_PIN=       PK_1;
const uint8_t CELL4_VOLTAGE_PIN=       PK_0;
const uint8_t CELL5_VOLTAGE_PIN=       PB_5;
const uint8_t CELL6_VOLTAGE_PIN=       PB_4;
const uint8_t CELL7_VOLTAGE_PIN=       PD_5;
const uint8_t CELL8_VOLTAGE_PIN=       PD_4;
const uint8_t cell_meas_pins[]=       {CELL1_VOLTAGE_PIN,CELL2_VOLTAGE_PIN,CELL3_VOLTAGE_PIN,CELL4_VOLTAGE_PIN,CELL5_VOLTAGE_PIN,CELL6_VOLTAGE_PIN,CELL7_VOLTAGE_PIN,CELL8_VOLTAGE_PIN};

//Control 
const uint8_t BUZZER_CONTROL_PIN=      PL_0;
const uint8_t FAN_CONTROL_PIN=         PP_5;
const uint8_t PACK_GATE_PIN=           PM_6;
const uint8_t LCD_TX_PIN=              PA_7;
const uint8_t LOGIC_SWITCH_PIN=        PK_6;

//Votlage measurements
const int VOLTS_MIN=                   2400; //mV
const int VOLTS_MAX=                   4200; //mV
const int PACK_V_ADC_MIN=              0000; //bits           need ADC values
const int PACK_V_ADC_MAX=              0000; //bits

const int CELL_UNDERVOLTAGE=           2650; //mV

//Current measurements
const int CURRENT_MAX=                 200000; //mA
const int CURRENT_MIN=                 -196207; //mA
const int OVERCURRENT=                 100000; //mA
const int CURRENT_ADC_MIN=             0;      //bits         need ADC values
const int CURRENT_ADC_MAX=             4096;   //bits

const int LOW_OVERCURRENT=             100000;  //mA
const int MED_OVERCURRENT=             110000;  //mA
const int HIGH_OVERCURRENT=            115000;  //mA
const int KILL_CURRENT=             120000; //mA

//Assorted delays
const int DEBOUNCE_DELAY=              10; //ms