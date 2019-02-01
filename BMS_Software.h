// Battery Managment System (BMS) Software /////////////////////////////////////////
//
// Created for 2019 Rover by: Jacob Lipina, jrlwd5
//
//
// Functions ///////////////////////////////////////////////////////////////////////

void getMainCurrent(RC_BMSBOARD_MAINIMEASmA_DATATYPE &main_current)
{
	float adc_reading;

  	adc_reading = analogRead(PACK_I_MEAS);
	main_current= 1000 * map(adc_reading, ADC_MIN, ADC_MAX, CURRENT_MIN, CURRENT_MAX); //??are constants from cpp understood in this header file? Can I mult function result by 1000? Is there a better way to get mA by defining variables differently?

	return;
}

void getCellVoltage(RC_BMSBOARD_VMEASmV_DATATYPE cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT])
{
  for(int i = 0; i<RC_BMSBOARD_VMEASmV_DATACOUNT; i++)
  {
    cell_voltage[i]=i;
  }
}

void getBattTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE &batt_temp)
{

}
