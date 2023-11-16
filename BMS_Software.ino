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

uint16_t notifyOverCurrent[] = {500, 100, 250, 30000};
uint8_t notify_index = 0;

uint16_t notifyOverCurrent[] = {500, 100, 250, 30000};
uint8_t notify_index = 0;

uint16_t notifyOverCurrent[] = {500, 100, 250, 30000};
uint8_t notify_index = 0;

uint16_t notifyOverCurrent[] = {500, 100, 250, 30000};
uint8_t notify_index = 0;

void notifyError() {
    if (overcurrent) {
        if (current_time - last_buzz_time > notifyOverCurrent[notify_index]) {
            if (i%2 == 0) {
                digitalWrite(BUZZER, HIGH);
            } else {
                digitalWrite(BUZZER, LOW);
            }
            last_buzz_time = current_time;
            notify_index++;
        }
    }

    // repeat for other errors
}

void notifyLowVoltage() // below 2.7 voltage
{
    if (LowVoltage) {
            if (current_time - last_buzz_time > notifyOverCurrent[notify_index]) {
                if (i%2 == 0) {
                    digitalWrite(BUZZER, HIGH);
                } else {
                    digitalWrite(BUZZER, LOW);
                }
                last_buzz_time = current_time;
                notify_index++;
            }
        }

    // repeat for other errors
}


void notifyUnderVoltage() // below 2.5 voltage
{
    if (UnderVoltage) {
                if (current_time - last_buzz_time > notifyOverCurrent[notify_index]) {
                    if (i%2 == 0) {
                        digitalWrite(BUZZER, HIGH);
                    } else {
                        digitalWrite(BUZZER, LOW);
                    }
                    last_buzz_time = current_time;
                    notify_index++;
                }
            }
}

void loop() {
    //update temp reading
    void readBatterTemp(){
        float temp = analogRead(ROOM_TEMP);
    }
    
    // turn off rover, estop
    rovecomm_packet packet = RoveComm.read();
    switch (packet.data_id) {
        // Increment servo target by provided value
        case RC_TRAININGSOFTWARE_SERVOINCREMENT_DATA_ID:
        {
            int16_t data = *((int16_t*) packet.data);
            
            roverOff(data);
            break;
        }
}