#include <Energia.h>
#include "RoveComm.h"
//sets up RoveComm

//LED control pins
#define SW_IND_PIN              PF_1
#define SW_ERR_IND_PIN          PM_3
#define V_OUT_IND_PIN           PH_2
#define FANS_IND_PIN            PH_3

//Sensor pins
#define TEMP_SENSE_PIN          PD_1
#define V_OUT_SENSE_PIN         PD_0
#define PACK_I_SENSE_PIN        PN_4
#define LOGIC_POWER_SENSE_PIN   PK_7

//Cell voltages
#define CELL1_VOLTAGE_PIN       PK_3
#define CELL2_VOLTAGE_PIN       PK_2
#define CELL3_VOLTAGE_PIN       PK_1
#define CELL4_VOLTAGE_PIN       PK_0
#define CELL5_VOLTAGE_PIN       PB_5
#define CELL6_VOLTAGE_PIN       PB_4
#define CELL7_VOLTAGE_PIN       PD_5
#define CELL8_VOLTAGE_PIN       PD_4

//Control 
#define BUZZER_CONTROL_PIN      PL_0
#define FAN_CONTROL_PIN         PP_5
#define PACK_GATE_PIN           PM_6
#define LCD_TX_PIN              PA_7
#define LOGIC_SWITCH_PIN        PK_6