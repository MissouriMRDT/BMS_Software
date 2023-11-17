#include "PinAssignments.h"
#include "BMS_SOFTWARE.h"

void setup () {
    Serial.begin(115200);
    Serial.println("BMS Setup")

    //RoveComm
    Serial.prinln("RoveComm Initializing...");
    RoveComm.begin(RC_BMS_FIRSTOCTET, RC_BMS_SECONDOCTET, RC_BMS_THIRDOCTET, RC_BMS_FOURTHOCTET, &TCPServer); 
    Serial.println("Complete."); 

}

void loop() {
    
    // turn off rover, estop
    rovecomm_packet packet = RoveComm.read();
    switch (packet.data_id) {
        // Increment servo target by provided value
        case RC_TRAININGSOFTWARE_SERVOINCREMENT_DATA_ID:
        {
            int16_t data = *((int16_t*) packet.data);
            
            eStop(data);
            break;
        }
    }

    current = mapAnalog(CURRENT_SENSE, ZERO_CURRENT, MAX_CURRENT, ZERO_CURRENT_ANALOG, MAX_CURRENT_ANALOG);
    //Check for Overcurrent
    if (current>=MAX_CURRENT) {
        errorOvercurrent();
    }

    temp - mapAnalog(TEMP, ROOM_TEMP_C, OTHER_TEMP_C, ROOM_TEMP_ANALOG, OTHER_TEMP_ANALOG);
    //Check for Overheat
    if (temp >= MAX_TEMP) {
        errorOverHeat();
    } else {
        digitalWrite(BUZZER, LOW);
    }

    //Check for Cell Undervoltage and Cell Critical
    for (uint8_t i = 0; i < 8; i++) {
        cell_voltages[i] = mapAnalog(cell_voltage_pins[i], ZERO_VOLTS, OTHER_VOLTS, ZERO_VOLTS_ANALOG, OTHER_VOLTS_ANALOG);
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (cell_voltages[i] <= CELL_CRITICAL_THRESHOLD) {
            errorCellCritical();
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (cell_voltages[i] <= CELL_UNDERVOLT_THRESHOLD) {
            errorCellUndervoltage();
        }
    }
}

//FUNCTIONS//

float mapAnalog(uint8_t pin, float units1, float units2, uint16_t analog1, uint16_t analog2) {
    float m = (units2 - units1) / (analog2 - analog1);
    return ((analogRead(pin) - analog1) * m) + units1;
}

void eStop() {
    digitalWrite(LOGIC_SWITCH_INPUT, HIGH);
    //beep bc bms is on, but everything else off
    while (true) {
        // smoke detector beep pattern
        beep on
        delay(1000); //check with malikai
        beep off
        delay(30000);

        // Check if cell goes critical?
    }
}

void roverRestart() {
    digitalWrite(LOGIC_SWITCH_INPUT, LOW);
    beep // also check
    delay(1000); //check with malikai
    digitalWrite(LOGIC_SWITCH_INPUT, HIGH);
    off beep // turn off buzzer
}

void roverSuicide() {
    beep 
    delay(200);
    digitalWrite(GATE, HIGH);
}

void errorOvercurrent() {
    RoveComm.write(RC_BMS_OVERCURRENT_DATA_ID, RC_BMS_OVERCURRENT_DATA_COUNT, /*overcurrent variable (temporary)*/);
    uint32_t current_time = millis();
    if ((current_time - lastOvercurrentErrorTimeStamp) >= TENTHOUSAND) {
        roverRestart();
        lastOvercurrentErrorTimeStamp = current_time;
    } else {
        roverSuicide(); //Call 988 :(
    }
}

void errorCellUndervoltage() {
    RoveComm.write(RC_BMS_CELLUNDERVOLT_DATA_ID, RC_BMS_CELLUNDERVOLT_DATA_COUNT, /*cellundervoltage variable (temporary)*/);
    eStop();
}

void errorCellCritical() {
    RoveComm.write(RC_BMS_CELLCRITICAL_DATA_ID, RC_BMS_CELLCRITICAL_DATA_COUNT);
    roverSuicide(); //Call 988
}

void errorOverHeat() {
    uint32_t current_time = millis();

    if (current_time - lastOverheatWriteTimeStamp > TELEMETRY_PERIOD) {
        RoveComm.write(RC_BMS_OVERHEAT_DATA_ID, RC_BMS_OVERHEAT_DATA_COUNT, temp);
        lastOverheatWriteTimeStamp = current_time;
    }

    if (current_time - lastBuzzTimeStamp > notifyOverheat[notifyOverheatIndex]) {
        digitalWrite(BUZZER, (notifyOverheatIndex%2));
        lastBuzzTimeStamp = current_time;
        notifyOverheatIndex++;
        if (notifyOverheatIndex >= NOTIFYOVERHEAT_LENGTH) {
            notifyOverheatIndex = 0;
        }
    }
}