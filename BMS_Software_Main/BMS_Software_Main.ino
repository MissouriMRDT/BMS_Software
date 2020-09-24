#include "BMS_Software_Main.h"
#include "RoveComm.h"

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial3.begin(9600); //starting comms with LCD display
  
  //RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
  delay(ROVECOMM_DELAY);
  
  setInputPins();
  setOutputPins();
  setOutputStates();

  time_cycle_start = millis(); //time program started
  time_cycle_now = 0;
  num_of_loops = 0 ;
}


void loop() 
{
  // put your main code here, to run repeatedly:
  getTemperature();
  //average 
  //time_cycle_now = millis(); //updates constantly
  //if((time_cycle_now - time_cycle_start)> 1000)
  //{
  //  time_cycle_start = millis(); //resets the time loop
   // sendRoveCommTelemetry(); //takes the averages of values needing to send to basestation
  //}
  //else
  //{
  //  num_of_loops ++; //increments the # of loops
  //}
  //main_current_average;
  //cell_adc_average[i];
  
  //rovecomm_packet packet;
  
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
  pinMode(PACK_OUT_CTRL_PIN,     OUTPUT);
  pinMode(LOGIC_SWITCH_CTRL_PIN, OUTPUT);
  pinMode(BUZZER_CTRL_PIN,       OUTPUT);
  pinMode(FAN_CTRL_PIN,          OUTPUT);
  pinMode(FAN_PWR_IND_PIN,      OUTPUT);
  pinMode(SW_IND_PIN,           OUTPUT);
  pinMode(SW_ERR_IND_PIN,       OUTPUT);
}

void setOutputStates()
{
  digitalWrite(PACK_OUT_CTRL_PIN,      HIGH);
  digitalWrite(LOGIC_SWITCH_CTRL_PIN,  LOW);
  digitalWrite(BUZZER_CTRL_PIN,        LOW);
  digitalWrite(FAN_CTRL_PIN,           LOW);
  digitalWrite(FAN_PWR_IND_PIN,       LOW);
  digitalWrite(SW_IND_PIN,            LOW);
  digitalWrite(SW_ERR_IND_PIN,        LOW);
}

void getTemperature()
{
  
  int adc_reading = analogRead(TEMP_degC_MEAS_PIN);
  int temp_average = map(adc_reading, TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX);
  Serial.println("Temperature :");
  Serial.println(temp_average);
  Serial.println();
}
void getCellVoltage(float cell_adc_average[])
{
  for(int i = 0; i < 8 ; i++)
  { 
    cell_adc_average[i]+= analogRead(CELL_MEAS_PINS[i]);        // adds cell voltages to an array of 8 (for average to be taken later)
  }
}

void getMainCurrent(float main_current_average)
{
  main_current_average += analogRead(PACK_I_MEAS_PIN); 
}

void sendRoveCommTelemetry(int num_of_loops, float main_current_average, float cell_adc_average[], float pack_v_average, float temp_average)
{
  main_current_average /= num_of_loops;
  //Rovecomm.write(RC_BMSBOARD_MAINIMEASmA_DATAID, main_current_average);
  num_of_loops = 0;
}
