#include "BMS_Software_Main.h"
#include "RoveComm.h"

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial3.begin(9600);
  //startScreen();
  RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
  delay(ROVECOMM_DELAY);

  setInputPins();
  setOutputPins();
  setOutputStates();
}

void loop() 
{
  // put your main code here, to run repeatedly: 
  float main_current;
  float cell_voltages[RC_BMSBOARD_CELL_VMEAS_DATACOUNT];
  float pack_out_voltage;
  float batt_temp;
  
  rovecomm_packet packet;

  //Serial.println();
  getMainCurrent(main_current);
  OverCurrentCheck();
  reactOverCurrent();

  getCellVoltage(cell_voltages);
  reactUnderVoltage();
  reactLowVoltage(cell_voltages);

  getOutVoltage(pack_out_voltage);
  reactEstopReleased();
  reactForgottenLogicSwitch();

  getBattTemp(batt_temp);
  reactOverTemp();
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

//Main Current Read and Send to Basestation
void getMainCurrent(float &main_current)
{
  main_current = ((map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX)));
  Rovecomm.write(RC_BMSBOARD_MAINIMEASmA_DATAID, main_current);
  
  if(main_current > OVERCURRENT)
  {
    delay(DEBOUNCE_DELAY);
    if(map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX) > OVERCURRENT)
    {
     
      overcurrent_state = true;

     if(num_overcurrent == 1)
      {
        num_overcurrent++;
      }
    }
  }
  return;
}

void getCellVoltage(float &cell_voltage[RC_BMSBOARD_CELL_VMEAS_DATACOUNT])   //CellV_Meas
{
  for(int i = 0; i < RC_BMSBOARD_CELL_VMEAS_DATACOUNT ; i++)
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
   
    cell_voltage[i] = ((map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX)));
    
    if(i > 1)
    {
      cell_voltage[i] -= 00;
    }
    if ((cell_voltage[i] > CELL_VOLTS_MIN) && (cell_voltage[i] < CELL_UNDERVOLTAGE))
    {
      delay(DEBOUNCE_DELAY);

      adc_reading = analogRead(CELL_MEAS_PINS[i]);

      if (adc_reading < CELL_V_ADC_MIN)
      {
        adc_reading = CELL_V_ADC_MIN;
      }
      if (adc_reading > CELL_V_ADC_MAX)
      {
        adc_reading = CELL_V_ADC_MAX;
      }
      if((map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX) <= CELL_UNDERVOLTAGE)
            && (map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX) > CELL_VOLTS_MIN))
      {
        cell_undervoltage_state = 1
        RoveComm.write(RC_BMSBOARD_CELL_UNDERVOLTAGE_DATAID, cell_undervoltage_state);
      }
    }
  }
  Rovecomm.write(RC_BMSBOARD_CELL_VMEAS_DATAID, cell_voltages[RC_BMSBOARD_CELL_VMEAS_DATACOUNT]);
  return;
}

void getOutVoltage(float &pack_out_voltage)  
{
  int adc_reading = analogRead(PACK_V_MEAS_PIN);
  pack_out_voltage = ((map(adc_reading, PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)));
  if (pack_out_voltage > PACK_SAFETY_LOW)
  {
    forgotten_logic_switch = false;
    time_switch_forgotten = 0;
    num_out_voltage_loops = 0;
  }
  if(pack_out_voltage < PACK_EFFECTIVE_ZERO)
  {
    delay(DEBOUNCE_DELAY);

    if(((map(analogRead(PACK_V_MEAS_PIN), PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX))) < PACK_EFFECTIVE_ZERO)
    {
      forgotten_logic_switch = true;
      estop_released_beep = false;
      num_out_voltage_loops++;
    }
  }
  return;
}

void getBattTemp(float &batt_temp)
{
  int adc_reading = analogRead(TEMP_degC_MEAS_PIN);
  
  if(adc_reading > TEMP_ADC_MAX)
  {
    adc_reading = TEMP_ADC_MAX;
  }
  if (adc_reading < TEMP_ADC_MIN)
  {
    adc_reading = TEMP_ADC_MIN;
  }

  meas_batt_temp[num_meas_batt_temp] = ((map(adc_reading, TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX)));
  num_meas_batt_temp ++;

  batt_temp = ((map(analogRead(TEMP_degC_MEAS_PIN), TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX)));

  if(num_meas_batt_temp % NUM_TEMP_AVERAGE == 0)
  {
    for(int i = 0; i < NUM_TEMP_AVERAGE; i++)
    {
      batt_temp += meas_batt_temp[i];
    } 
    batt_temp /= NUM_TEMP_AVERAGE;
    num_meas_batt_temp = 0; 
    batt_temp_avail = true; 
  }
  if (batt_temp_avail == true)
  {
    if(batt_temp < TEMP_THRESHOLD)
    {
      overtemp_state = false;
    }
    if (batt_temp > TEMP_THRESHOLD)
    {
      delay(DEBOUNCE_DELAY);
  
      if(map(analogRead(TEMP_degC_MEAS_PIN), TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX) > TEMP_THRESHOLD)
      {
        overtemp_state = true;
      }
    }
  }
  return;
}


//change shit
void reactOverCurrent()
{
  if (overcurrent_state == false)
  {
    //error_report[RC_BMSBOARD_PACK_OVERCURRENT_DATACOUNT] = RC_BMSBOARD_PACK_OVERCURRENT;
  }
  if(overcurrent_state == true)
  {
    if(num_overcurrent == 0)
    {
      //error_report[RC_BMSBOARD_PACK_OVERCURRENT_DATACOUNT] = RC_BMSBOARD_ERROR_OVERCURRENT;
      //RoveComm.write(RC_BMSBOARD_PACK_OVERCURRENT_DATAID, error_report);
      delay(ROVECOMM_DELAY);
      
      digitalWrite(PACK_OUT_CTRL_PIN, LOW);
    
      time_of_overcurrent = millis(); 
      num_overcurrent++;
    
      //notifyOverCurrent();
    }
    else if(num_overcurrent == 1)
    {
      if(millis() >= (time_of_overcurrent + RESTART_DELAY))
      {
        digitalWrite(PACK_OUT_CTRL_PIN, HIGH);
      }
      if(millis() >= (time_of_overcurrent + RESTART_DELAY + RECHECK_DELAY))
      {
        overcurrent_state = false;
        num_overcurrent = 0;
        time_of_overcurrent = 0;
      }
    }
    else
    {
      //error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_OVERCURRENT;
      //RoveComm.write(RC_BMSBOARD_ERROR_HEADER, error_report);
      delay(ROVECOMM_DELAY);
      
      digitalWrite(PACK_OUT_CTRL_PIN, LOW);

      //notifyOverCurrent();

      digitalWrite(LOGIC_SWITCH_CTRL_PIN, HIGH); //BMS Suicide   
    }
  }
  return;
}

void reactUnderVoltage() 
{
  if((pack_undervoltage_state == true) || cell_undervoltage_state == true)
  {
    //RoveComm.write(RC_BMSBOARD_ERROR_HEADER, error_report);
    delay(DEBOUNCE_DELAY);

    digitalWrite(PACK_OUT_CTRL_PIN, LOW);
    //notifyUnderVoltage();
    digitalWrite(LOGIC_SWITCH_CTRL_PIN, HIGH); //BMS Suicide
  }
  return;
}

void reactOverTemp()
{
  if(overtemp_state == true && fans_on == false) 
  { 
    fans_on = true;               
    digitalWrite(FAN_CTRL_PIN, HIGH);
    digitalWrite(FAN_PWR_IND_PIN, HIGH);
  }
  if(overtemp_state == false && fans_on == true)
  {
    fans_on = false;
    digitalWrite(FAN_CTRL_PIN, LOW);
    digitalWrite(FAN_PWR_IND_PIN, LOW);
  }
  return;
}

void reactEstopReleased()
{
  if(forgotten_logic_switch == false && estop_released_beep == false)
  {
    estop_released_beep = true;
    notifyEstopReleased();
  }
}

void reactLowVoltage(float &cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT])
{
  if((cell_voltage[0] > PACK_UNDERVOLTAGE) && (cell_voltage[0] <= PACK_LOWVOLTAGE) && (low_voltage_state = false))//first instance of low voltage
  { 
    low_voltage_state = true;
    //notifyLowVoltage();
    time_of_low_voltage = millis();
    num_low_voltage_reminder = 1;
  }/
  else if((cell_voltage[0] > PACK_UNDERVOLTAGE) && (cell_voltage[0] <= PACK_LOWVOLTAGE) && (low_voltage_state = true))//following instances of low voltage
  {
    if(millis() >= (time_of_low_voltage + (num_low_voltage_reminder * LOGIC_SWITCH_REMINDER)))
    { 
      //notifyLowVoltage();
      num_low_voltage_reminder++;
    }
  }
  return;
}

void setEstop(uint8_t data)
{
  if(data == 0)
  {
    digitalWrite(PACK_OUT_CTRL_PIN, LOW);

    notifyEstop();

    digitalWrite(LOGIC_SWITCH_CTRL_PIN, HIGH); //BMS Suicide
    //If BMS is not turned off here, the PACK_OUT_CTR_PIN would be low and there would be no way to get it high again without reseting BMS anyway.
  }
  else
  {
    digitalWrite(PACK_OUT_CTRL_PIN, LOW);
    notifyReboot();  
    delay(data * 1000); //Receiving delay in seconds so it needs to be converted to msec.

    digitalWrite(PACK_OUT_CTRL_PIN, HIGH);
  }
  return;
}

void notifyEstopReleased()
{
  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(25);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);

  return;
}

void notifyEstop() //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep
{
  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(75);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);

  return;
}

void notifyLogicSwitch() //Buzzer sound: beeep beeep
{
  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);

  return;
}

void notifyReboot() //Buzzer sound: beeep beeep
{
  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);
  delay(100);

  digitalWrite(BUZZER_CTRL_PIN, HIGH);
  digitalWrite(SW_ERR_IND_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_CTRL_PIN, LOW);
  digitalWrite(SW_ERR_IND_PIN, LOW);

  return;
}
