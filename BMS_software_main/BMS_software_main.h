#include <Energia.h>
#include "RoveComm.h"
//sets up RoveComm

//LED control pins
#define SW_IND              PF_1
#define SW_ERR_IND          PM_3
#define V_OUT_IND           PH_2
#define FANS_IND            PH_3

//Sensor pins
#define TEMP_SENSE          PD_1
#define V_OUT_SENSE         PD_0
#define PACK_I_SENSE        PN_4
#define LOGIC_POWER_SENSE   PK_7

//Cell voltages
#define CELL1_VOLTAGE       PK_3
#define CELL2_VOLTAGE       PK_2
#define CELL3_VOLTAGE       PK_1
#define CELL4_VOLTAGE       PK_0
#define CELL5_VOLTAGE       PB_5
#define CELL6_VOLTAGE       PB_4
#define CELL7_VOLTAGE       PD_5
#define CELL8_VOLTAGE       PD_4

//Control 
#define BUZZER_CONTROL      PL_0
#define FAN_CONTROL         PP_5
#define PACK_GATE           PM_6
#define LCD_TX              PA_7
#define LOGIC_SWITCH        PK_6