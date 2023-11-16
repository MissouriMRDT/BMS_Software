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


// Servo
#define SERVO_1_START 90
#define SERVO_1_MIN 0
#define SERVO_1_MAX 180
Servo Servo1;
void incrementServo(uint8_t& servo_target, int16_t inc, uint8_t max, uint8_t min);


// State Variables
uint8_t servo_1_target = SERVO_1_START;
float temp; // degrees C


#endif /* TRAININGSOFTWARE_H */