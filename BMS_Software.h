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


// State Variables

float temp; // degrees C

#define ROOM_TEMP_C 0 //celcius
#define ROOM_TEMP_ANALOG 0
#define OTHER_TEMP_C 0
#define OTHER_TEMP_ANALOG 0

void roverOff() {
    beep // also check
    delay(1000) //check with malikai
    digitalWrite(LOGIC_SWITCH_INPUT, LOW)
}

void roverRestart() {
    digitalWrite(LOGIC_SWITCH_INPUT, HIGH)
    beep // also check
    delay(1000) //check with malikai
    digitalWrite(LOGIC_SWITCH_INPUT, LOW)
    off beep // turn off buzzer
}

void roverRestart() {
    beep 
    delay(1000)
    turn off contactor
}

#endif /* TRAININGSOFTWARE_H */