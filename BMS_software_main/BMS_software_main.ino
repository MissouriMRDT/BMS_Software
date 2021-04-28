#include "BMS_software_main.h"


//code that will actually run
void setup()
{
    Serial.begin(9600);
    Serial3.begin(9600);

    setPinInputs();
    setPinOutputs();
    setPinOutputStates();
}

//loop need to decide order to run functions and how often i.e. any delays
void loop()
{
    getCellVoltage();
    delay(1000); //why is this here?
}


//PINSTATE Functions//////////////////////////////////////////////////////////////////////////////////
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

//getBLANK Functions///////////////////////////////////////////////////////////////////////////////////////////////
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
                estop();
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
                highBeep();
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
                medBeep();
            }
        }
        else if((cell_value > CELL_LOW_UNDERVOLTAGE) && (CELL_MED_UNDERVOLTAGE > cell_value))
        {
            delay(DEBOUNCE_DELAY);
            float cell_value = map(analogRead(cell_meas_pins[i]),CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX);
            if((cell_value > CELL_LOW_UNDERVOLTAGE) && (CELL_MED_UNDERVOLTAGE > cell_value))
            {
                lowBeep();
            }
        }
        cells[i] = cell_value/1000;
    }
    return cells;
}

float getPackVoltage()  
{
    float packVoltage = map(analogRead(V_OUT_SENSE_PIN),PACK_V_ADC_MIN,PACK_V_ADC_MAX,PACK_V_MIN,PACK_V_MAX);
    if(packVoltage > KILL_PACK_VOLTAGE)
    {
        delay(DEBOUNCE_DELAY);
        float packVoltage = map(analogRead(V_OUT_SENSE_PIN),PACK_V_ADC_MIN,PACK_V_ADC_MAX,PACK_V_MIN,PACK_V_MAX);
        if(packVoltage > KILL_PACK_VOLTAGE)
        {
            estop();
        }
    }
    else if((packVoltage > PACK_HIGH_UNDERVOLT) && (KILL_PACK_VOLTAGE > packVoltage))
    {
        delay(DEBOUNCE_DELAY);
        float packVoltage = map(analogRead(V_OUT_SENSE_PIN),PACK_V_ADC_MIN,PACK_V_ADC_MAX,PACK_V_MIN,PACK_V_MAX);
        if((packVoltage > PACK_HIGH_UNDERVOLT) && (KILL_PACK_VOLTAGE > packVoltage))
        {
            //insert RoveComm pack undervolt error
            highBeep();
        }
    }
    else if((packVoltage > PACK_MED_UNDERVOLT) && (PACK_HIGH_UNDERVOLT > packVoltage))
    {
        delay(DEBOUNCE_DELAY);
        float packVoltage = map(analogRead(V_OUT_SENSE_PIN),PACK_V_ADC_MIN,PACK_V_ADC_MAX,PACK_V_MIN,PACK_V_MAX);
        if((packVoltage > PACK_MED_UNDERVOLT) && (PACK_HIGH_UNDERVOLT > packVoltage))
        {
            //insert RoveComm pack undervolt error
            medBeep();
        }
    }
    else if((packVoltage > PACK_LOW_UNDERVOLT) && (PACK_MED_UNDERVOLT > packVoltage))
    {
        delay(DEBOUNCE_DELAY);
        float packVoltage = map(analogRead(V_OUT_SENSE_PIN),PACK_V_ADC_MIN,PACK_V_ADC_MAX,PACK_V_MIN,PACK_V_MAX);
        if((packVoltage > PACK_LOW_UNDERVOLT) && (PACK_MED_UNDERVOLT > packVoltage))
        {
            lowBeep();
        }  
    }
    return packVoltage;
}

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
           restart();
       }
       else if((packCurrent > HIGH_OVERCURRENT) && (KILL_CURRENT > packCurrent))
       {
            //insert RoveComm overcurrent error
            highBeep();
       }
       else if((packCurrent > MED_OVERCURRENT) && (HIGH_OVERCURRENT > packCurrent))
       {
            //insert RoveComm overcurrent error
            medBeep();
       }
       else if((packCurrent > LOW_OVERCURRENT) && (MED_OVERCURRENT > packCurrent))
       {
           lowBeep();
       }
   }
   return packCurrent;
}


//ESTOP & LOGIC SWITCH///////////////////////////////////////////////////////////////////////////////////////
void restart()
{
    digitalWrite(PACK_GATE_PIN, LOW);
    delay(3000); //ms
    digitalWrite(PACK_GATE_PIN, HIGH);
}

void estop()
{
    digitalWrite(PACK_GATE_PIN, LOW);
    estopBeep();
}


//Buzzer Functions///////////////////////////////////////////////////////////////////////
void lowBeep() //beep beep    beep beep    beep beep  x3
{
    for(int i=0; i<2; i++)
    {
        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        digitalWrite(SW_ERR_IND_PIN, HIGH);
        delay(BUZZER_SHORT);
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        digitalWrite(SW_ERR_IND_PIN, LOW);
        delay(BUZZER_LONG);
    }
}

void medBeep()  //beep  beep  beeeeep x3
{
    for(int i=0; i<2; i++)
    {
        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        digitalWrite(SW_ERR_IND_PIN, HIGH);
        delay(BUZZER_MED);
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        digitalWrite(SW_ERR_IND_PIN, LOW);
        delay(BUZZER_MED);

        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        digitalWrite(SW_ERR_IND_PIN, HIGH);
        delay(BUZZER_MED);
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        digitalWrite(SW_ERR_IND_PIN, LOW);
        delay(BUZZER_MED);

        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        digitalWrite(SW_ERR_IND_PIN, HIGH);
        delay(BUZZER_LONG);
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        digitalWrite(SW_ERR_IND_PIN, LOW);
        delay(BUZZER_MED);
    }
}

void highBeep()  //beeeeeeeep  x5
{
    for(int i=0; i<4; i++)
    {
        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        digitalWrite(SW_ERR_IND_PIN, HIGH);
        delay(BUZZER_LONG);
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        digitalWrite(SW_ERR_IND_PIN, LOW);
        delay(BUZZER_SHORT); 
    }
}

void estopBeep() //beep beep beep beep beep beep beep beep beep beep
{
    for(int i=0; i<4; i++)
    {
        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        digitalWrite(SW_ERR_IND_PIN, HIGH);
        delay(BUZZER_SHORT);
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        digitalWrite(SW_ERR_IND_PIN, LOW);
        delay(BUZZER_SHORT);
    }
}


void startScreen()
{
    Serial3.write('|'); //setting character
    Serial3.write('-'); //clear display

    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp
    Serial3.write(0X20); //sp

        Serial3.write(0xA1); //next line  ??

    Serial3.write(0X41); //A
    Serial3.write(0X55); //U
    Serial3.write(0X52); //R
    Serial3.write(0X4F); //O
    Serial3.write(0X52); //R
    Serial3.write(0X41); //A
    Serial3.write(0X20); //sp
    Serial3.write(0X49); //I
    Serial3.write(0X4E); //N
    Serial3.write(0X49); //I
    Serial3.write(0X54); //T
    Serial3.write(0X49); //I
    Serial3.write(0X41); //A
    Serial3.write(0X4C); //L
    Serial3.write(0X49); //I
    Serial3.write(0X5A); //Z
    Serial3.write(0X49); //I
    Serial3.write(0X4E); //N
    Serial3.write(0X47); //G
    Serial3.write(0X20); //sp

        Serial3.write(0xA1); //next line

    Serial3.write(0x23);//#
    Serial3.write(0x52);//R
    Serial3.write(0x4F);//O
    Serial3.write(0x56);//V
    Serial3.write(0x45);//E
    Serial3.write(0x53);//S
    Serial3.write(0x4F);//O
    Serial3.write(0x48);//H
    Serial3.write(0x41);//A
    Serial3.write(0x52);//R
    Serial3.write(0x44);//D
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x32); //2
    Serial3.write(0x30); //0
    Serial3.write(0x32); //2
    Serial3.write(0x31); //1
}

