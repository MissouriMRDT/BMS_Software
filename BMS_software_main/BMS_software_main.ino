#include "BMS_software_main.h"


//code that will actually run
void setup()
{
    Serial.begin(9600);

    setPinInputs();
    setPinOutputs();
    setPinOutputStates();
}

//loop
void loop()
{
    getCellVoltage();
    delay(1000);
}


//functions that will be called
void setPinInputs()
{
    pinMode(CELL1_VOLTAGE_PIN,      INPUT);
    pinMode(CELL2_VOLTAGE_PIN,      INPUT);
    pinMode(CELL3_VOLTAGE_PIN,      INPUT);
    pinMode(CELL4_VOLTAGE_PIN,      INPUT);
    pinMode(CELL5_VOLTAGE_PIN,      INPUT);
    pinMode(CELL6_VOLTAGE_PIN,      INPUT);
    pinMode(CELL7_VOLTAGE_PIN,      INPUT);
    pinMode(CELL8_VOLTAGE_PIN,      INPUT);

    pinMode(TEMP_SENSE_PIN,         INPUT);
    pinMode(V_OUT_SENSE_PIN,        INPUT);
    pinMode(PACK_I_SENSE_PIN,       INPUT);
    pinMode(LOGIC_POWER_SENSE_PIN,  INPUT);
}

void setPinOutputs()
{
    pinMode(SW_IND_PIN,             OUTPUT);
    pinMode(SW_ERR_IND_PIN,         OUTPUT);
    pinMode(V_OUT_IND_PIN,          OUTPUT);
    pinMode(FANS_IND_PIN,           OUTPUT);

    pinMode(BUZZER_CONTROL_PIN,     OUTPUT);  
    pinMode(FAN_CONTROL_PIN,        OUTPUT);
    pinMode(PACK_GATE_PIN,           OUTPUT);
    pinMode(LCD_TX_PIN,             OUTPUT);
    pinMode(LOGIC_SWITCH_PIN,       OUTPUT);
}

void setPinOutputStates()
{
    digitalWrite(BUZZER_CONTROL_PIN,    LOW);
    digitalWrite(FAN_CONTROL_PIN,       LOW);
    digitalWrite(PACK_GATE_PIN,         HIGH);
    digitalWrite(LOGIC_SWITCH_PIN,      LOW);
    digitalWrite(SW_IND_PIN,            LOW);
    digitalWrite(SW_ERR_IND_PIN,        LOW);
    digitalWrite(V_OUT_IND_PIN,         LOW);
    digitalWrite(FANS_IND_PIN,          LOW);
}

float* getCellVoltage()
{
    float cells[RC_BMSBOARD_CELLV_MEAS_DATA_COUNT] = {};

    for(int i=0; i<RC_BMSBOARD_CELLV_MEAS_DATA_COUNT; i++)
    {
        float cell_value = map(analogRead(cell_meas_pins[i]),CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX);
        if(cell_value > KILL_CELL_VOLTAGE)
        {
            delay(DEBOUNCE_DELAY);
            float cell_value = map(analogRead(cell_meas_pins[i]),CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX);
            if(cell_value > KILL_CELL_VOLTAGE)
            {
                //insert kill rove command
            }
        }
        else if((cell_value > CELL_HIGH_UNDERVOLTAGE) && (KILL_CELL_VOLTAGE > cell_value))
        {
            delay(DEBOUNCE_DELAY);
            float cell_value = map(analogRead(cell_meas_pins[i]),CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX);
            if((cell_value > CELL_HIGH_UNDERVOLTAGE) && (KILL_CELL_VOLTAGE > cell_value))
            {
                uint8_t undervolt = i;
                //insert RoveComm cell undervolt error
                //high warning beep
            }
        }
        else if((cell_value > CELL_MED_UNDERVOLTAGE) && (CELL_HIGH_UNDERVOLTAGE > cell_value))
        {
            delay(DEBOUNCE_DELAY);
            float cell_value = map(analogRead(cell_meas_pins[i]),CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX);
            if((cell_value > CELL_MED_UNDERVOLTAGE) && (CELL_HIGH_UNDERVOLTAGE > cell_value))
            {
                uint8_t undervolt = i;
                //insert RoveComm cell undervolt error
                //med warning beep
            }
        }
        else if((cell_value > CELL_LOW_UNDERVOLTAGE) && (CELL_MED_UNDERVOLTAGE > cell_value))
        {
            delay(DEBOUNCE_DELAY);
            float cell_value = map(analogRead(cell_meas_pins[i]),CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX);
            if((cell_value > CELL_LOW_UNDERVOLTAGE) && (CELL_MED_UNDERVOLTAGE > cell_value))
            {
                //low warning beep
            }
        }
        cells[i] = cell_value/1000;
    }
    return cells;
}

/*
float getPackVoltage()  //NOT FINISHED
{
    float packVoltage = map(analogRead(V_OUT_SENSE_PIN),PACK_V_ADC_MIN,PACK_V_ADC_MAX,PACK_V_MIN,PACK_V_MAX);
    if((packVoltage > ))
}
*/

float getPackCurrent()
{
   float packCurrent = map(analogRead(PACK_I_SENSE_PIN),CURRENT_ADC_MIN,CURRENT_ADC_MAX,CURRENT_MIN,CURRENT_MAX); 
   if((packCurrent > LOW_OVERCURRENT) && (MED_OVERCURRENT > packCurrent))
   {
       delay(DEBOUNCE_DELAY);
       float packCurrent = map(analogRead(PACK_I_SENSE_PIN),CURRENT_ADC_MIN,CURRENT_ADC_MAX,CURRENT_MIN,CURRENT_MAX); 
//consider moving the following into a different function that reacts to the data gather in the "getBLANK" function
       if(packCurrent > KILL_CURRENT)
       {
           //insert kill rover command
       }
       else if((packCurrent > HIGH_OVERCURRENT) && (KILL_CURRENT > packCurrent))
       {
            //insert RoveComm overcurrent error
            //high warning beep
       }
       else if((packCurrent > MED_OVERCURRENT) && (HIGH_OVERCURRENT > packCurrent))
       {
            //insert RoveComm overcurrent error
            //med warning beep
       }
       else if((packCurrent > LOW_OVERCURRENT) && (MED_OVERCURRENT > packCurrent))
       {
           //low warning beep
       }
   }
   return packCurrent;
}