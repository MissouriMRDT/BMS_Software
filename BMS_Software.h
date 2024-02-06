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

// Important constants
#define NUM_CELLS 6
#define RESTART_DELAY 1000
#define FAN_TEMP_THRESHOLD 35
#define MAX_TEMP 65
#define CELL_UNDERVOLT_THRESHOLD 2.7
#define CELL_CRITICAL_THRESHOLD 2.5
#define MAX_CURRENT 80
#define LCD_UPDATE_PERIOD 500

float temp = 0;
#define ROOM_TEMP_C 18 //celcius
#define ROOM_TEMP_ANALOG 200
#define OTHER_TEMP_C 85
#define OTHER_TEMP_ANALOG 360

float current = 0;
#define ZERO_CURRENT 0 //Amps
#define ZERO_CURRENT_ANALOG 0
#define MAX_CURRENT_ANALOG 0
#define OTHER_CURRENT 0
#define OTHER_CURRENT_ANALOG 0

float cell_voltages[8];
uint8_t cell_voltage_pins[8] = {CELL_SENSE_1, CELL_SENSE_2, CELL_SENSE_3, CELL_SENSE_4, CELL_SENSE_5, CELL_SENSE_6, CELL_SENSE_7, CELL_SENSE_8};
#define ZERO_VOLTS 0 //Volts
#define ZERO_VOLTS_ANALOG 0
#define OTHER_VOLTS 4.2
#define OTHER_VOLTS_ANALOG 1023

float packVoltage = 0;


uint32_t lastOvercurrentErrorTimestamp = 0;
uint32_t lastLCDupdate = 0;
#define TENTHOUSAND 10000 //BEMIS

#define NOTIFYOVERHEAT_LENGTH 6
uint32_t lastBuzzTimestamp = 0;
uint32_t lastOverheatWriteTimestamp = 0;
uint16_t notifyOverheat[NOTIFYOVERHEAT_LENGTH] = {200, 200, 200, 200, 400, 30000};
uint8_t notifyOverheatIndex = 0;

void roverEStop();
void roverRestart();
void roverSuicide();

void errorOvercurrent();
void errorCellUndervoltage();
void errorCellCritical();
void errorOverHeat();

float mapAnalog();

#endif /* TRAININGSOFTWARE_H */