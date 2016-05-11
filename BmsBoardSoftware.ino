// Standard C
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//Energia
#include <EasyTransfer.h>

const uint8_t BMS_BATTERY_PACK_OFF          = 1040;
const uint8_t BMS_BATTERY_PACK_OFF_REBOOT   = 1041;

const uint8_t BMS_BATTERY_PACK_VOLTAGE         = 1072;
const uint8_t BMS_BATTERY_PACK_CURRENT         = 1073;

//////////////////////////////////////////////RoveBoard
// Pins
const int ESTOP_CNTRL_P6_0    = 2;
const int ESTOP_P1_6          = 15;

const int AMPS_PACK_P5_5      = 30;
const int VOLTS_PACK_P5_3     = 61;

const int DISPLAY_LED_LCD_ON_P6_6    = 36; //Active Low

// TLC6C5912-Q1
const int LED_GRAPH_CLEAR_P10_0      = 0;  //TODO 
const int LED_GRAPH_SERIAL_P10_2     = 0;  //TODO 
const int LED_GRAPH_RCK_P10_1        = 0;  //TODO 
const int LED_GRAPH_SRCK_P10_3       = 0;  //TODO 

// I2/SPI CharacterADafruit product : ID 292
const int LCD_DATA        = 0;
const int LCD_LATCH       = 0;
const int LCD_CLOCK       = 0;

/////////////////////////////////////////////RoveBoard
// Tiva1294C RoveBoard Specs
const float VCC                 = 3.3;       //volts
const float ADC_MAX             = 4096;      //bits
const float ADC_MIN             = 0;         //bits
float adc_reading = 0;

//////////////////////////////////////////////Sensor
// ACS_722 IC Sensor Specs 
const float SENSOR_SENSITIVITY   = 0.0066;    //volts/amp
const float SENSOR_SCALE         = 0.5;      //volts/amp
const float SENSOR_BIAS          = VCC * SENSOR_SCALE;

const float CURRENT_MAX          = (VCC - SENSOR_BIAS) / SENSOR_SENSITIVITY;
const float CURRENT_MIN          = -SENSOR_BIAS / SENSOR_SENSITIVITY;
float current_reading            = 0;

const int ESTOP_PACK_AMPS_MAX_THRESHHOLD = 180;

const int REBOOT_DELAY     = 5000;
const int REBOOT_TRY_COUNT = 3;

const int ANALOG_DEBOUNCE_DELAY  = 10;
const int DIGITAL_DEBOUNCE_DELAY = 10;
const int DOUBLE_DEBOUNCE_DELAY  = 1;

typedef struct 
{
  int16_t data_id;
  float pack_voltage;
  float pack_current;
} recieve_data;

typedef struct 
{
  int16_t data_id; 
} send_data;

recieve_data receive_powerboard_command;
send_data    send_powerboard_telem;

EasyTransfer FromPowerboard, ToPowerboard;


float mapFloats(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}//end fnctn

// Checks the pin for bouncing voltages to avoid false positives
bool singleAnalogDebounce(int bouncing_pin, int max_amps_threshold)
{
  int adc_threshhold = mapFloats(max_amps_threshold, CURRENT_MIN, CURRENT_MAX, ADC_MIN, ADC_MAX);
  
  if( analogRead(bouncing_pin) > adc_threshhold)
  {  
    Task_sleep(ANALOG_DEBOUNCE_DELAY);
    //delay(ANALOG_DEBOUNCE_DELAY);
    
    if( analogRead(bouncing_pin) > adc_threshhold)
    {
       return true;
    }//end if
  }// end if 
  return false;
}//end fntcn


// the setup routine runs once when you press reset
void setup() 
{   
  Serial.begin(9600);
  
  FromPowerboard.begin(details(receive_powerboard_command), &Serial); 
  ToPowerboard.begin(details(send_powerboard_telem), &Serial);

  // Control Pins are outputs
  pinMode(ESTOP_CNTRL_P6_0, OUTPUT);
  
  // Turn on everything when we begin
  digitalWrite(ESTOP_CNTRL_P6_0, LOW);
  
  // Estop on everything when we begin
  attachInterrupt(ESTOP_P1_6, estop, CHANGE);
}//end setup



//Loop
/////////////////////////////////////////////Powerboard Loop Forever
void loop() 
{
  if(singleAnalogDebounce( AMPS_PACK_P5_5, ESTOP_PACK_AMPS_MAX_THRESHHOLD))
  {
    digitalWrite(ESTOP_CNTRL_P6_0, LOW);
    //delay(REBOOT_DELAY);
    
    for(int reboot_tries = 0; reboot_tries < REBOOT_TRY_COUNT; reboot_tries++)
    {
       digitalWrite(ESTOP_CNTRL_P6_0, HIGH);
     
       if(singleAnalogDebounce(AMPS_PACK_P5_5, ESTOP_PACK_AMPS_MAX_THRESHHOLD))
       {   
         digitalWrite(ESTOP_CNTRL_P6_0, LOW);
         //reboot_failing = true;
       }else{
         break;
       }//end if
    }//end for  
  }//end if
  
  if( FromPowerboard.receiveData() )
  {   
    switch (receive_powerboard_command.data_id)
      { 
      case 0:
        break;
        
      case BMS_BATTERY_PACK_OFF:
        digitalWrite(ESTOP_CNTRL_P6_0, LOW);
        break;
        
      case BMS_BATTERY_PACK_OFF_REBOOT:
        digitalWrite(ESTOP_CNTRL_P6_0, LOW);
        delay(REBOOT_DELAY);
        digitalWrite(ESTOP_CNTRL_P6_0, HIGH);
        break;     
      default:
      //Serial.print("Unrecognized data :");
      //Serial.println(data);
      break; 
      }//endswitch
  }//end if
  
}//end loop



void estop()
{    
  // Keep from interrupt ourselves
  
  delay(DIGITAL_DEBOUNCE_DELAY); 
  
  if( digitalRead(ESTOP_P1_6) == HIGH)
  {  
    //delay(DOUBLE_DEBOUNCE_DELAY); 
    
    if( digitalRead(ESTOP_P1_6) == HIGH )
    {
       digitalWrite(ESTOP_CNTRL_P6_0, LOW);
    }//end if
    
  }else{
    
    //delay(DOUBLE_DEBOUNCE_DELAY);  
    
    if( digitalRead(ESTOP_P1_6) == LOW )
    {
       digitalWrite(ESTOP_CNTRL_P6_0, HIGH);
    }//end if 
  }//end if  
  
}//end functn



