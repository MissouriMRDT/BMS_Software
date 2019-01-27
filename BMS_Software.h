// Battery Managment System (BMS) Software /////////////////////////////////////////
//
// Created for 2019 Rover by: Jacob Lipina, jrlwd5
//
//
// Functions ///////////////////////////////////////////////////////////////////////

void value_meas(uint8_t pin, float VALUE_MIN, float VALUE_MAX, int DataID, other Rovecomm stuff)
{
	float adc_reading = 0;
	float value_reading = 0;
	int value_reading_milli = 0;

	adc_reading = analogRead(pin);
	value_reading = 3 * map(adc_reading, ADC_MIN, ADC_MAX, VALUE_MIN, VALUE_MAX); //Value mult by 3 to convert the base unit, say volts, to milli, say millivolts
	value_reading_milli = int(value_reading);
	//Insert Rovecomm command to send voltage to RED
	delay(ROVECOMM_DELAY);
	return;
}

