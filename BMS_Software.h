#ifndef BMSSOFTWARE_H
#define BMSSOFTWARE_H

#include "PinAssignments.h"
#include "RoveComm.h"

// RoveComm
EthernetServer TCPServer(RC_ROVECOMM_ETHERNET_TCP_PORT);
RoveCommEthernet RoveComm;

// Telemetry
#define TELEMETRY_PERIOD 1000000
IntervalTimer Telemetry;
void telemetry();

float mapAnalog();

#define MAX_TEMP 65
#define FAN_TEMP_THRESHOLD 0 //???
#define CELL_UNDERVOLT_THRESHOLD 2.7
#define CELL_CRITICAL_THRESHOLD 2.5
#define MAX_CURRENT 80

float temp = 0;
#define ROOM_TEMP_C 21 //celcius
#define ROOM_TEMP_ANALOG 0
#define OTHER_TEMP_C 0
#define OTHER_TEMP_ANALOG 0

float current = 0;
#define ZERO_CURRENT 0 //Amps
#define ZERO_CURRENT_ANALOG 0
#define OTHER_CURRENT 0
#define OTHER_CURRENT_ANALOG 0

float cell_voltages[8];
uint8_t cell_voltage_pins[8] = {CELL_SENSE_1, CELL_SENSE_2, CELL_SENSE_3, CELL_SENSE_4, CELL_SENSE_5, CELL_SENSE_6, CELL_SENSE_7, CELL_SENSE_8};
#define ZERO_VOLTS 0 //Volts
#define ZERO_VOLTS_ANALOG 0
#define OTHER_VOLTS 0
#define OTHER_VOLTS_ANALOG 0 

void eStop();

void roverRestart();

void roverSuicide();


uint32_t lastOvercurrentErrorTimeStamp = 0;
#define TENTHOUSAND 10000 //BEMIS

void errorOvercurrent();

void errorCellUndervoltage();

void errorCellCritical();

#define NOTIFYOVERHEAT_LENGTH 6
uint32_t lastBuzzTimeStamp = 0;
uint32_t lastOverheatWriteTimeStamp = 0;
uint16_t notifyOverheat[NOTIFYOVERHEAT_LENGTH] = {50, 50, 50, 50, 50, 30000};
uint8_t notifyOverheatIndex = 0;

void errorOverHeat();

#endif /* TRAININGSOFTWARE_H */

//temp cutoff 65 C