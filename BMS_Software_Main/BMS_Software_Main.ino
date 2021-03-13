#include "BMS_Software_Main.h"
#include "RoveComm.h"

void setup() 
{
  Serial.begin(9600);   //
  Serial3.begin(9600); //comms with LCD display
  
  RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
  delay(ROVECOMM_DELAY);
  
  setInputPins();
  setOutputPins();
  setOutputStates();
}


void loop() 
{
  //listen for rovecomm packets
  rovecomm_packet packet;
  packet = RoveComm.read();
  
  //if estop, go directly to estop function
  if (packet.data_id = RC_BMSBOARD_BMSSTOP_DATA_ID)
  {
    estop(packet.data[0]);
  }

  //get telemetry and send it

  //check for errors
  
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

void setOutputStates() //is the pin on or off? must be set for all digital pins
{
  digitalWrite(PACK_OUT_CTRL_PIN,      HIGH);
  digitalWrite(LOGIC_SWITCH_CTRL_PIN,  LOW);
  digitalWrite(BUZZER_CTRL_PIN,        LOW);
  digitalWrite(FAN_CTRL_PIN,           LOW);
  digitalWrite(FAN_PWR_IND_PIN,       LOW);
  digitalWrite(SW_IND_PIN,            LOW);
  digitalWrite(SW_ERR_IND_PIN,        LOW);
}


void estop(uint8_t time)
{
  if (time > 0)
  {
    digitalWrite(PACK_OUT_CTRL_PIN, LOW);
    delay(time);
    digitalWrite(PACK_OUT_CTRL_PIN, HIGH);
  }
}

void SendRovecommPackets()
{
  float temp = getTemperature();
  float cells[RC_BMSBOARD_VMEASmV_DATACOUNT] = getCellVoltages();
  float pack_current = getCurrent()p;
}
// get temp for battery pack and if too high, turn on fans
float getTemperature()
{
  temp = analog.read(TEMP_degC_MEAS_PIN);
  serial.print(temp);
  return temp;
}

// get the individual voltage of 8 cells and put in an array
float cells[RC_BMSBOARD_VMEASmV_DATACOUNT] getCellVoltages()
{
  cells[RC_BMSBOARD_VMEASmV_DATACOUNT] = {}
  for a in range(RC_BMSBOARD_VMEASmV_DATACOUNT)
  {
    float cell = map(analogRead(CELL_MEAS_PINS[a]), PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX);
    cells[a] = cell;
    // check if cell is low
    if (cell < CELL_SAFETY_LOW)
    {
      delay(DEBOUNCE_DELAY);

    }
  }
}

// get pack current
float getCurrent()
{

}
