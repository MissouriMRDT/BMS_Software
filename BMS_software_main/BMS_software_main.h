#include <Energia.h>
#include "RoveComm.h"
//sets up RoveComm

//LED control pins
const uint8_t SW_IND_PIN              PF_1;
const uint8_t SW_ERR_IND_PIN          PM_3;
const uint8_t V_OUT_IND_PIN           PH_2;
const uint8_t FANS_IND_PIN            PH_3;

//Sensor pins
const uint8_t TEMP_SENSE_PIN          PD_1;
const uint8_t V_OUT_SENSE_PIN         PD_0;
const uint8_t PACK_I_SENSE_PIN        PN_4;
const uint8_t LOGIC_POWER_SENSE_PIN   PK_7;

//Cell voltage pins
const uint8_t CELL1_VOLTAGE_PIN       PK_3;
const uint8_t CELL2_VOLTAGE_PIN       PK_2;
const uint8_t CELL3_VOLTAGE_PIN       PK_1;
const uint8_t CELL4_VOLTAGE_PIN       PK_0;
const uint8_t CELL5_VOLTAGE_PIN       PB_5;
const uint8_t CELL6_VOLTAGE_PIN       PB_4;
const uint8_t CELL7_VOLTAGE_PIN       PD_5;
const uint8_t CELL8_VOLTAGE_PIN       PD_4;
const uint8_t cell_meas_pins[]=       {CELL1_VOLTAGE_PIN,CELL2_VOLTAGE_PIN,CELL3_VOLTAGE_PIN,CELL4_VOLTAGE_PIN,CELL5_VOLTAGE_PIN,CELL6_VOLTAGE_PIN,CELL7_VOLTAGE_PIN,CELL8_VOLTAGE_PIN}

//Control 
const uint8_t BUZZER_CONTROL_PIN=      PL_0;
const uint8_t FAN_CONTROL_PIN=         PP_5;
const uint8_t PACK_GATE_PIN=           PM_6;
const uint8_t LCD_TX_PIN=              PA_7;
const uint8_t LOGIC_SWITCH_PIN=        PK_6;

float cell1_voltage;

//lowercase changing, CAPITAL NOT CHANGING