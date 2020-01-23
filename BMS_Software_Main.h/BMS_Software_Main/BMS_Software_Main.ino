#include "BMS_Software_Main.h"

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial3.begin(9600);
  startScreen();
  RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
  delay(ROVECOMM_DELAY)

  setInputpins();
  setOutputPins();
  setOutputStates();
}

void loop() 
{
  // put your main code here, to run repeatedly: 


  
  
  
}

void setInputPins()
{
  pinMode(PACK_I_MEAS_PIN,      INPUT);
  pinMode(PACK_V_MEAS_PIN,      INPUT);
  pinMode(LOGIC_V_MEAS_PIN,     INPUT);
  pinMode(TEMP_degC_MEAS_PIN,   INPUT);
  pinMode(C1_V_MEAS_PIN,        INPUT);
  pinMode(C2_V_MEAS_PIN,        INPUT);
  pinMode(C3_V_MEAS_PIN,        INPUT);
  pinMode(C4_V_MEAS_PIN,        INPUT);
  pinMode(C5_V_MEAS_PIN,        INPUT);
  pinMode(C6_V_MEAS_PIN,        INPUT);
  pinMode(C7_V_MEAS_PIN,        INPUT);
  pinMode(C8_V_MEAS_PIN,        INPUT);
}

void setOutputPins()
{
  pinMode(PACK_OUT_CTR_PIN,     OUTPUT);
  pinMode(LOGIC_SWITCH_CTR_PIN, OUTPUT);
  pinMode(BUZZER_CTR_PIN,       OUTPUT);
  pinMode(FAN_CTR_PIN,          OUTPUT);
  pinMode(FAN_PWR_IND_PIN,      OUTPUT);
  pinMode(SW_IND_PIN,           OUTPUT);
  pinMode(SW_ERR_IND_PIN,       OUTPUT);
}

void setOutputStates()
{
  digitalWrite(PACK_OUT_CTR_PIN,      HIGH);
  digitalWrite(LOGIC_SWITCH_CTR_PIN,  LOW);
  digitalWrite(BUZZER_CTR_PIN,        LOW);
  digitalWrite(FAN_CTR_PIN,           LOW);
  digitalWrite(FAN_PWR_IND_PIN,       LOW);
  digitalWrite(SW_IND_PIN,            LOW);
  digitalWrite(SW_ERR_IND_PIN,        LOW);
}


void getMainCurrent(int32_t &main_current)
{
  main_current = ((map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX)*1069)/1000);//*950)/1000);
  
  if(main_current > OVERCURRENT)
  {
    delay(DEBOUNCE_DELAY);
    if(map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX) > OVERCURRENT)

}
