#include "D:\Github\Bms_Software\BMS_Software.h"

void setup() {
  // put your setup code here, to run once:
  setInput();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  getAdcValue();
}

void setInput(){
    pinMode(TEMP_degC_MEAS_PI, INPUT);
  pinMode(SW_ERR_PIN, OUTPUT);
}

void getAdcValue(){
  digitalWrite(SW_ERR_PIN,HIGH);
    Serial.print("  temp ");
    Serial.print("      ");
    Serial.print(analogRead(TEMP_degC_MEAS_PI));
    Serial.println(" ");
    delay(500);
}
