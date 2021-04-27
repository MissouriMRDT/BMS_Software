#include <Energia.h>
#include "RoveComm.h"
//sets up RoveComm

//PIN NAMES//
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

//CONSTANTS//
//Cell voltage measurements
const int CELL_VOLTS_MIN=              2400; //mV
const int CELL_VOLTS_MAX=              4200; //mV
const int CELL_V_ADC_MIN=              0000; //bits
const int CELL_V_ADC_MAX=              0000; //bits
//Undervolt cell values
const int CELL_LOW_UNDERVOLTAGE=       3700; //mV
const int CELL_MED_UNDERVOLTAGE=       3500; //mV
const int CELL_HIGH_UNDERVOLTAGE=      3300; //mV
const int KILL_CELL_VOLTAGE=           3000; //mV

//Pack voltage measurements
const int PACK_V_MIN=                  24000; //mV ???              
const int PACK_V_MAX=                  33600; //mV
const int PACK_V_ADC_MIN=              0000; //bits           need ADC values
const int PACK_V_ADC_MAX=              0000; //bits
//Undervolt pack values
const int PACK_LOW_UNDERVOLT=          29600; //mV
const int PACK_MED_UNDERVOLT=          28000; //mV
const int PACK_HIGH_UNDERVOLT=         26400; //mV
const int KILL_PACK_VOLTAGE=           24000; //mV

//Pack current measurements
const int CURRENT_MAX=                 200000; //mA
const int CURRENT_MIN=                 -196207; //mA
const int CURRENT_ADC_MIN=             0;      //bits         need ADC values
const int CURRENT_ADC_MAX=             4096;   //bits
//Undercurrent values
const int LOW_OVERCURRENT=             100000;  //mA
const int MED_OVERCURRENT=             110000;  //mA
const int HIGH_OVERCURRENT=            115000;  //mA
const int KILL_CURRENT=                120000; //mA

//Temp measurements
const int TEMP_Min=                    00; // *C
const int TEMP_MAX=                    00; // *C
const int TEMP_ADC_MIN=                0000; //bits
const int TEMP_ADC_MAX=                0000; // bits
//Temp superhot values

//Assorted delays
const int DEBOUNCE_DELAY=              10; //ms