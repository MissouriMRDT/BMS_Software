#include "BMStest.h"


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  setInputPins();
  setOutputPins();
  setOutputStates();
}

void loop() 
{
  // put your main code here, to run repeatedly:
 getCellVoltage(cell_voltage);
 delay(1000);  
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
  Serial.println("setInputPins Complete");
}

void setOutputPins()
{
  pinMode(PACK_OUT_CTRL_PIN,     OUTPUT);
  pinMode(LOGIC_SWITCH_CTRL_PIN, OUTPUT);
  pinMode(BUZZER_CTRL_PIN,       OUTPUT);
  pinMode(FAN_CTRL_PIN,          OUTPUT);
  pinMode(FAN_PWR_IND_PIN,      OUTPUT);
  pinMode(SW_IND_PIN,           OUTPUT);
  pinMode(SW_ERR_IND_PIN,       OUTPUT);
  Serial.println("setOutputPins Complete");
  return;
}

void setOutputStates()
{
  digitalWrite(PACK_OUT_CTRL_PIN,      LOW);
  digitalWrite(LOGIC_SWITCH_CTRL_PIN,  HIGH);
  digitalWrite(BUZZER_CTRL_PIN,        LOW);
  digitalWrite(FAN_CTRL_PIN,           LOW);
  digitalWrite(FAN_PWR_IND_PIN,       LOW);
  digitalWrite(SW_IND_PIN,            LOW);
  digitalWrite(SW_ERR_IND_PIN,        LOW);
  Serial.println("setOutputStates Complete");
  return;
}

void getCellVoltage(float cell_voltage[])
{
  Serial.println("Cell Voltages ADC:");
  Serial.println(analogRead(C1_V_MEAS_PIN));
  Serial.println(analogRead(C2_V_MEAS_PIN));
  Serial.println(analogRead(C3_V_MEAS_PIN));
  Serial.println(analogRead(C4_V_MEAS_PIN));
  Serial.println(analogRead(C5_V_MEAS_PIN));
  Serial.println(analogRead(C6_V_MEAS_PIN));
  Serial.println(analogRead(C7_V_MEAS_PIN));
  Serial.println(analogRead(C8_V_MEAS_PIN));
  Serial.println();

  for(int i = 0; i < 8 ; i++)
  {
    int adc_reading = analogRead(CELL_MEAS_PINS[i]);
    if(adc_reading < CELL_V_ADC_MIN)
    {
      adc_reading = CELL_V_ADC_MIN;
    }
    if(adc_reading > CELL_V_ADC_MAX)
    {
      adc_reading = CELL_V_ADC_MAX;
    }
    cell_voltage[i] = (map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX));
    Serial.println(cell_voltage[i]);
  }
  return;
}

void getPackVoltage()
{
  Serial.println("PackVoltage :");
  Serial.println(analogRead(PACK_V_MEAS_PIN));
  Serial.println();
  return;
}

void getLogicVoltage()
{
  Serial.println("LogicVoltage :");
  Serial.println(analogRead(LOGIC_V_MEAS_PIN));
  Serial.println();
  return;
}
 
void getPackCurrent()
{
  Serial.println("PackCurrent :");
  Serial.println(analogRead(PACK_I_MEAS_PIN));
  Serial.println();
  return;
}
