#include "PinAssignments.h"
#include "BMS_Software.h"
#include "LCD.h"
#include <cstdint>

void setup () {
    Serial.begin(115200);
    Serial.println("BMS Setup");

    //I/O Pins
    pinMode(BUZZER, OUTPUT);
    pinMode(CONTACTOR, OUTPUT);
    pinMode(ESTOP, OUTPUT);
    pinMode(ERR_LED, OUTPUT);
    pinMode(FAN, OUTPUT);

    digitalWrite(BUZZER, LOW);
    digitalWrite(CONTACTOR, LOW);
    digitalWrite(ESTOP, HIGH);
    digitalWrite(ERR_LED, LOW);
    digitalWrite(FAN, LOW);

    //Initialize LCD
    LCD_init();

    //RoveComm
    Serial.println("RoveComm Initializing...");
    RoveComm.begin(RC_BMSBOARD_FIRSTOCTET, RC_BMSBOARD_SECONDOCTET, RC_BMSBOARD_THIRDOCTET, RC_BMSBOARD_FOURTHOCTET, &TCPServer); 
    Serial.println("Complete."); 

    //Telemetry
    Telemetry.begin(telemetry, TELEMETRY_PERIOD);
}

void loop() {
    /*
    //Update LCD with new data every 500 milliseconds
    uint32_t current_time = millis();
    if (current_time - lastLCDupdate > LCD_UPDATE_PERIOD) {
        LCD_update(temp, packVoltage, cell_voltages);
        lastLCDupdate = current_time;
    }
    */

    current = mapAnalog(CURRENT_SENSE, ZERO_CURRENT, OTHER_CURRENT, ZERO_CURRENT_ANALOG, OTHER_CURRENT_ANALOG);
    //Check for Overcurrent
    /*
    if (current>=MAX_CURRENT) {
        errorOvercurrent();
    }
    */

    temp = mapAnalog(TEMP, ROOM_TEMP_C, OTHER_TEMP_C, ROOM_TEMP_ANALOG, OTHER_TEMP_ANALOG);
    //Check for Overheat
    if (temp >= MAX_TEMP) {
        errorOverHeat();
    }

    //Calculate Cell and Pack Voltages
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        cell_voltages[i] = mapAnalog(cell_voltage_pins[i], ZERO_VOLTS, OTHER_VOLTS, ZERO_VOLTS_ANALOG, OTHER_VOLTS_ANALOG);
    }
    delay(10);
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        float new_voltage = mapAnalog(cell_voltage_pins[i], ZERO_VOLTS, OTHER_VOLTS, ZERO_VOLTS_ANALOG, OTHER_VOLTS_ANALOG);
        if (cell_voltages[i] < new_voltage) {
            cell_voltages[i] = new_voltage;
        }
        packVoltage += cell_voltages[i];
    }

    //Check for Cell Undervoltage and Cell Critical
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        if (cell_voltages[i] <= CELL_CRITICAL_THRESHOLD) {
            if (i != 0) // cell 1  shutoff temporarily disabled - MONITOR CLOSELY
            errorCellCritical();
        }
    }
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        if (cell_voltages[i] <= CELL_UNDERVOLT_THRESHOLD) {
            if (i != 0) // cell 1  shutoff temporarily disabled - MONITOR CLOSELY
            errorCellUndervoltage();
        }
    }
  

    //Turn on Fan if temp is above a certain threshold
    if (temp > FAN_TEMP_THRESHOLD) {
        digitalWrite(FAN, HIGH);
    }

    //Check for incoming RoveComm packets
    rovecomm_packet packet = RoveComm.read();

    switch (packet.data_id) {
        //Estop
        case RC_BMSBOARD_ESTOP_DATA_ID:
        {
            //int16_t data = *((int16_t*) packet.data);
            roverEStop();
            break;
        }

        //Suicide call 988 :(
        case RC_BMSBOARD_SUICIDE_DATA_ID:
        {
            //int16_t data = *((int16_t*) packet.data);
            roverSuicide();
            break;
        }

        //Reboot
        case RC_BMSBOARD_REBOOT_DATA_ID:
        {
            //int16_t data = *((int16_t*) packet.data);
            roverRestart();
        }
    }
}

//FUNCTIONS//

void telemetry() {
    RoveComm.write(RC_BMSBOARD_PACKCURRENT_DATA_ID, RC_BMSBOARD_PACKCURRENT_DATA_COUNT, current); //Current Draw
    packVoltage = 0;
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        packVoltage += cell_voltages[i];
    }
    RoveComm.write(RC_BMSBOARD_PACKVOLTAGE_DATA_ID, RC_BMSBOARD_PACKVOLTAGE_DATA_COUNT, packVoltage); //Pack voltage
    RoveComm.write(RC_BMSBOARD_CELLVOLTAGE_DATA_ID, RC_BMSBOARD_CELLVOLTAGE_DATA_COUNT, cell_voltages);
    RoveComm.write(RC_BMSBOARD_PACKTEMP_DATA_ID, RC_BMSBOARD_PACKTEMP_DATA_COUNT, temp); //Temperature
}

float mapAnalog(uint8_t pin, float units1, float units2, uint16_t analog1, uint16_t analog2) {
    float slope = (units2 - units1) / (analog2 - analog1);
    return ((analogRead(pin) - analog1) * slope) + units1;
}

void roverEStop() {
    digitalWrite(ESTOP, LOW);
    //beep bc bms is on, but everything else off
    while (true) {
        // smoke detector beep pattern
        digitalWrite(BUZZER, HIGH);
        delay(1000);
        digitalWrite(BUZZER, LOW);
        delay(30000);

        // Check if cell goes critical?
    }
}

void roverRestart() {
    digitalWrite(ESTOP, LOW);
    digitalWrite(BUZZER, HIGH);
    delay(RESTART_DELAY);
    digitalWrite(ESTOP, HIGH);
    digitalWrite(BUZZER, LOW);
}

void roverSuicide() {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(CONTACTOR, HIGH);
}

uint8_t dummy = 0;

void errorOvercurrent() {
    RoveComm.writeReliable(RC_BMSBOARD_OVERCURRENT_DATA_ID, RC_BMSBOARD_OVERCURRENT_DATA_COUNT, dummy);
    uint32_t current_time = millis();
    if ((current_time - lastOvercurrentErrorTimestamp) >= TENTHOUSAND) {
        roverRestart();
        lastOvercurrentErrorTimestamp = current_time;
    } else {
        roverSuicide(); //Call 988 :(
    }
}

void errorCellUndervoltage() {
    RoveComm.writeReliable(RC_BMSBOARD_CELLUNDERVOLTAGE_DATA_ID, RC_BMSBOARD_CELLUNDERVOLTAGE_DATA_COUNT, dummy);
    roverEStop();
}

void errorCellCritical() {
    RoveComm.writeReliable(RC_BMSBOARD_CELLCRITICAL_DATA_ID, RC_BMSBOARD_CELLCRITICAL_DATA_COUNT, dummy);
    roverSuicide(); //Call 988
}

void errorOverHeat() {
    uint32_t current_time = millis();

    if (current_time - lastOverheatWriteTimestamp > TELEMETRY_PERIOD) {
        RoveComm.writeReliable(RC_BMSBOARD_PACKOVERHEAT_DATA_ID, RC_BMSBOARD_PACKOVERHEAT_DATA_COUNT, dummy);
        lastOverheatWriteTimestamp = current_time;
    }

    //non blocking beep
    if (current_time - lastBuzzTimestamp > notifyOverheat[notifyOverheatIndex]) {
        digitalWrite(BUZZER, (notifyOverheatIndex%2));
        lastBuzzTimestamp = current_time;
        notifyOverheatIndex++;
        if (notifyOverheatIndex >= NOTIFYOVERHEAT_LENGTH) {
            notifyOverheatIndex = 0;
        }
    }
}