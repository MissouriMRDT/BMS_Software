/*//////////////////////////////////////////////////////////////////////
@file:			BMSSoftware.ino
@author:		Emily "Ellis" Sansone
@description:	This program continually monitors the state of the rover's battery voltages, temperature, and current output.
                It reports this data back to the Power Board, then determines whether to shut off power based on the data or the power board's response.
//////////////////////////////////////////////////////////////////////*/

#include <SPI.h>
#include <EasyTransfer.h>
#include "bms.h" 
#include <stdint.h>
#include <stdbool.h>
//#include <ltc6803.h>

/*
///This code, as of 11/12/16 does the following:
    switches pack on an off based on current (high limit) and voltage (low limit), and command from Power Board
    updates LED fual gauge values
    switches off logic power while idle for long period
    reports pack voltage and current on command from PB
To do:
    look into map() funtion
    temperature reading
    fan control
    read voltages from LTC6803, report back to PB and/or shut off pack
    fix values for fuel gauge
*/
///This function sets each IO pin to the correct mode
void setup()
{
    pinMode(PACK_GATE,		OUTPUT);
    pinMode(LOGIC_SWITCH,	OUTPUT);
    pinMode(FAN_CTRL_1,		OUTPUT);
    pinMode(FAN_CTRL_2,		OUTPUT);
    pinMode(FAN_CTRL_3,		OUTPUT);
    pinMode(FAN_CTRL_4,		OUTPUT);
    pinMode(BUZZER,			OUTPUT);
    pinMode(LED_RCK,		OUTPUT);
    pinMode(LED_CLR,		OUTPUT);
    pinMode(LED_SRCK,		OUTPUT);
    pinMode(GAUGE_ON,		OUTPUT);
    pinMode(LED_SER_IN,		OUTPUT);
    pinMode(LED1,OUTPUT);
    pinMode(PACK_I_MEAS,	INPUT);
    pinMode(V_CHECK_ARRAY,	INPUT);
    pinMode(V_CHECK_OUT,	INPUT);
    
    digitalWrite(PACK_GATE, HIGH); //turn off the pack while we get started
    digitalWrite(LOGIC_SWITCH, LOW); //make sure rocker switch is on for BMS logic power
    digitalWrite(LED1,HIGH);
    Serial.begin(9600);
    Serial.println("Setup complete");
    establishContact();
}

///main loop function
int loop()
{
 /*   unsigned long IdleTimer=0;    //timer to count how long the rover has been sitting with ESTOP triggered
    //ltc6803_write_config(CDC0); //Initialize the LTC6803s to standby comparator duty cycle
    float adc_reading=0;
    
    char incomingByte = 0;
    setup();
    Serial.println("Starting loop...");
    
    while(TRUE)
    {
//over-current check
        if(singleDebounce(PACK_I_MEAS, AMP_OVERCURRENT,AMPS_MIN,AMPS_MAX))  
        {
            digitalWrite(PACK_GATE, LOW);
            delay(REBOOT_DELAY);		//wait a little
            // try to turn pack on, three times
            Serial.println("Over-current triggered");
            for(int reboot_tries = 0; reboot_tries < REBOOT_TRY_COUNT; reboot_tries++)
            {
                digitalWrite(PACK_GATE, HIGH);
                if(singleDebounce(PACK_I_MEAS, AMP_OVERCURRENT,AMPS_MIN,AMPS_MAX))
                {   
                  digitalWrite(PACK_GATE, LOW);  //failed reboot, shut back off
                  Serial.println("Failed to reboot, turning back off");
                  IdleTimer=millis(); //start timer
                }
                else
                { 
                  Serial.println("Successful reboot");
                  break;// successful reboot, resume looping
                }       
            }		
        }
//check for low voltage
        if(!singleDebounce(V_CHECK_ARRAY, BATTERY_LOW, VOLTS_MIN, VOLTS_MAX))
        { 
            if(PACK_GATE==HIGH) //pack voltage is below threshold and gate is open
            {
            //send low voltage warning to PB
                Serial.print("Low Voltage: ");
                adc_reading = analogRead(V_CHECK_ARRAY);          //get the reading
                Serial.println(scale(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, VOLTS_MAX));
            }
        }
//check for dead battery
        if(!singleDebounce(V_CHECK_ARRAY, BATTERY_LOW_CRIT, VOLTS_MIN, VOLTS_MAX))
        { 
            if(PACK_GATE==HIGH) //pack voltage is below threshold and gate is open
            {
                Serial.print("Dead Battery! ");
                adc_reading = analogRead(V_CHECK_ARRAY);          //get the reading
                Serial.println(scale(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, VOLTS_MAX));
                
                digitalWrite(PACK_GATE, LOW);
                IdleTimer=millis(); //start timer
                delay(REBOOT_DELAY);		//wait a little
                // try to turn pack on, three times
                for(int reboot_tries = 0; reboot_tries < REBOOT_TRY_COUNT; reboot_tries++)
                {
                    digitalWrite(PACK_GATE, HIGH);
                    if(!singleDebounce(V_CHECK_ARRAY, BATTERY_LOW_CRIT, VOLTS_MIN, VOLTS_MAX))
                    {   
                      digitalWrite(PACK_GATE, LOW);  //failed reboot, shut back off
                      Serial.println("Failed to reboot, turning back off");
                      IdleTimer=millis(); //start timer
                    }
                    else
                    { 
                      Serial.println("Successful reboot");
                      break;// successful reboot, resume looping
                    }       
                }
            }            
        }
//check for commands from PB, to either shut off pack or return data
        //if(FromPowerboard.receiveData())
        if(Serial.available() > 0)
        {   
            incomingByte = Serial.read();
            
            //switch (powerboard_command_rx.data_id)
            switch(incomingByte)
            { 
                case 0: //do nothing
                    break;
                case 1: //PACK_OFF: //No reboot, shut down forever
                    digitalWrite(PACK_GATE, LOW);
                    Serial.println("complete shutdown");
                    break;
                case 2: //PACK_OFF_REBOOT: //Shut off power once, wait, back on
                    digitalWrite(PACK_GATE, LOW);
                    Serial.println("Rebooting...");
                    delay(REBOOT_DELAY);
                    digitalWrite(PACK_GATE, HIGH);
                    Serial.println("Reboot complete");
                    break;
                case 3: //PACK_AMPS: //send back the current reading
                    powerboard_telem_tx.data_id = PACK_AMPS;        //put in the data id
                    adc_reading = analogRead(PACK_I_MEAS);          //get the reading
                    powerboard_telem_tx.data = scale(adc_reading, ADC_MIN, ADC_MAX, AMPS_MIN, AMPS_MAX);  //convert the reading to amps and pack it up
                    //ToPowerboard.sendData();  //send it off
                    Serial.print("Pack current reading: ");
                    Serial.println(powerboard_telem_tx.data);
                    break;
                case 4: //PACK_VOLTS:
                    powerboard_telem_tx.data_id = PACK_VOLTS;        //put in the data id
                    adc_reading = analogRead(V_CHECK_ARRAY);          //get the reading
                    powerboard_telem_tx.data = scale(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, VOLTS_MAX);  //convert the reading to volts and pack it up
                    //ToPowerboard.sendData();  //send it off
                    Serial.print("Pack voltage reading: ");
                    Serial.println(powerboard_telem_tx.data);
                    break;
                /*case CELL_1_VOLT
                    powerboard_telem_tx.data_id = CELL_1_VOLT;
                    ltc6803Conv();             //Begin a voltage measurement
                    ltc6803ReadIn();           //Read the voltages back
                    powerboard_telem_tx.data =cells[0][0];
                    ToPowerboard.sendData();  //send it off
                    break;
                case CELL_2_VOLT
                    powerboard_telem_tx.data_id = CELL_2_VOLT;
                    ltc6803Conv();             //Begin a voltage measurement
                    ltc6803ReadIn();           //Read the voltages back
                    powerboard_telem_tx.data =cells[0][1];
                    ToPowerboard.sendData();  //send it off
                    break;
                case CELL_3_VOLT
                    powerboard_telem_tx.data_id = CELL_3_VOLT;
                    
                    break;
                case CELL_4_VOLT
                    powerboard_telem_tx.data_id = CELL_4_VOLT;
                    
                    break;
                case CELL_5_VOLT
                    powerboard_telem_tx.data_id = CELL_5_VOLT;
                    
                    break;
                case CELL_6_VOLT
                    powerboard_telem_tx.data_id = CELL_6_VOLT;
                    
                    break;
                case CELL_7_VOLT
                    powerboard_telem_tx.data_id = CELL_7_VOLT;
                    
                    break;
                case CELL_8_VOLT
                    powerboard_telem_tx.data_id = CELL_8_VOLT;
                    
                    break;
                *//*
                default: //NO HABLO INGLES
                    //Serial.print("Unrecognized data :");
                    //Serial.println(data);
                    break; 
            }
        }
//check for idle time limit
        if((millis()-IdleTimer)>IDLE_DELAY)
        {
            if(PACK_GATE==LOW)
            {
                digitalWrite(LOGIC_SWITCH, HIGH);
                //delay(DEBOUNCE_DELAY); //this shouldn't be necessary, as energizing this switch should disconnect the logic power, but.
                //digitalWrite(LOGIC_SWITCH, LOW);
            }
        }	
/*
//fuel gauge update
        voltsToFuelGauge_LED(analogRead(V_CHECK_ARRAY),LED_SER_IN, LED_SRCK, LED_RCK, MSBFIRST, 0);
        delay(LOOP_DELAY);*//*
    }
    */
}

///Returns true if pin's value is over the threshold
bool singleDebounce(int bouncing_pin, int value_threshold, float value_min, float value_max)
{
  int adc_threshold = (value_threshold - value_min) * (ADC_MAX - ADC_MIN) / (value_max - value_min) + value_min;
  if(analogRead(bouncing_pin) > adc_threshold)
  {  
    delay(DEBOUNCE_DELAY);
    if(analogRead(bouncing_pin) > adc_threshold)//double check to avoid transient triggering
    {
       return true;
    }
  }
  return false;
}
///scales the input value x from analog input range (0 to 3.3) to actual values (Pack voltage or current)
float scale(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/*
///Converts the Pack Voltage to a binary value that can be displayed on the Fuel Gauge.
void voltsToFuelGauge_LED(float volts_reading, int data_pin, int clock_pin, int latch_pin, uint8_t bit_order = MSBFIRST, uint32_t delay_micros = 0)
{
  uint16_t gauge_pattern=0;
  
  if(volts_reading > VOLTS_MAX/10 && volts_reading < VOLTS_MAX/9)
    gauge_pattern = 0x0001; //000000 00000001
  if(volts_reading > VOLTS_MAX/9 && volts_reading < VOLTS_MAX/8)
    gauge_pattern = 0x0003; //000000 00000011
  if(volts_reading > VOLTS_MAX/8 && volts_reading < VOLTS_MAX/7)
    gauge_pattern = 0x0007; //000000 00000111
  if(volts_reading > VOLTS_MAX/7 && volts_reading < VOLTS_MAX/6)
    gauge_pattern = 0x000F; //000000 00001111
  if(volts_reading > VOLTS_MAX/6 && volts_reading < VOLTS_MAX/5)
    gauge_pattern = 0x001F; //000000 00011111
  if(volts_reading > VOLTS_MAX/5 && volts_reading < VOLTS_MAX/4)
    gauge_pattern = 0x003F; //000000 00111111
  if(volts_reading > VOLTS_MAX/4 && volts_reading < VOLTS_MAX/3)
    gauge_pattern = 0x007F; //000000 01111111
  if(volts_reading > VOLTS_MAX/3 && volts_reading < VOLTS_MAX/2)
    gauge_pattern = 0x00FF; //000000 11111111
  if(volts_reading > VOLTS_MAX/2 && volts_reading < VOLTS_MAX)
    gauge_pattern = 0x01FF; //000001 11111111
  if(volts_reading > VOLTS_MAX)
    gauge_pattern = 0x03FF; //000011 11111111

  // latch low so the LED's don't change during shift
  digitalWrite(latch_pin, LOW);
    
  // shift out the bit
  shiftOut(data_pin, clock_pin, MSBFIRST, gauge_pattern);  

  // latch high so LED's light up  
  digitalWrite(latch_pin, HIGH);
  
  // pause before next value:
  delayMicroseconds(delay_micros);
}*/

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("waiting"); // send a capital A
    delay(300);
  }
}
