//code that will actually run
void setup(){

Serial.begin(9600);

setPinInputs();
setPinOutputs();

}

void loop(){

}

//functions that will be called
void setPinInputs(){

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

void setPinOutputs(){

pinMode(SW_IND_PIN,             OUTPUT);
pinMode(SW_ERR_IND_PIN,         OUTPUT);
pinMode(V_OUT_IND_PIN,          OUTPUT);
pinMode(FANS_IND_PIN,           OUTPUT);

pinMode(BUZZER_CONTROL_PIN,     OUTPUT);
pinMode(FAN_CONTROL_PIN,        OUTPUT);
pinMode(PACK_GATE_PIN           OUTPUT);
pinMode(LCD_TX_PIN,             OUTPUT);
pinMode(LOGIC_SWITCH_PIN,       OUTPUT);

}