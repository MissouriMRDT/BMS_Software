// Standard C
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//Energia
#include <EasyTransfer.h>
#include <SPI.h>



const uint8_t BMS_BATTERY_PACK_ON_OFF          = 1040;
const uint8_t BMS_BATTERY_PACK_ON_OFF_REBOOT   = 1041;

const uint8_t BMS_BATTERY_PACK_VOLTAGE         = 1072;
const uint8_t BMS_BATTERY_PACK_CURRENT         = 1073;


const int ESTOP_DELAY_MILLIS  = 250; 

volatile int digital_reading  = HIGH;
volatile int estop_occured_flag  = 0;

const int ESTOP_THRESHHOLD_AMPS = 19;
const int ESTOP_THRESHHOLD_VOLTS = 19;
const int DEBOUNCE_DELAY = 10;

//////////////////////////////////////////////RoveBoard
// Pins
const int ESTOP_CNTRL_P6_0    = 2;
const int ESTOP_P5_5          = 30;
const int MIKE_TODO           = 30;

// Tiva1294C RoveBoard Specs
const float VCC                 = 3.3;       //volts
const float ADC_MAX             = 4096;      //bits
const float ADC_MIN             = 0;         //bits

//////////////////////////////////////////////Sensor
// ??? IC Sensor Specs 
const float SENSOR_SENSITIVITY   = 0.125;    //volts/amp
const float SENSOR_SCALE         = 0.1;      //volts/amp
const float SENSOR_BIAS          = VCC * SENSOR_SCALE;

const float CURRENT_MAX = (VCC - SENSOR_BIAS) / SENSOR_SENSITIVITY;
const float CURRENT_MIN = -SENSOR_BIAS / SENSOR_SENSITIVITY;

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

/*float mapFloats(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}//end fnctn

// Checks the pin for bouncing voltages to avoid false positives
bool singleDebounce(int bouncing_pin, int max_amps_threshold)
{
  int adc_threshhold = map(max_amps_threshold, CURRENT_MIN, CURRENT_MAX, ADC_MIN, ADC_MAX);
  
  if( analogRead(bouncing_pin) > adc_threshhold)
  {  
    delay(DEBOUNCE_DELAY);
    
    if( analogRead(bouncing_pin) > adc_threshhold)
    {
       return true;
    }//end if
  }// end if 
  return false;
}//end fntcn*/



// the setup routine runs once when you press reset
void setup() 
{   
  Serial1.begin(9600);
  
  FromPowerboard.begin(details(receive_powerboard_command), &Serial1);  
  ToPowerboard.begin(details(send_powerboard_telem), &Serial1);

  // Control Pins are outputs
  pinMode(ESTOP_CNTRL_P6_0, OUTPUT);

  // Turn on everything when we begin
  digitalWrite(ESTOP_CNTRL_P6_0, HIGH);
  
  // Estop on everything when we begin
  attachInterrupt(ESTOP_P5_5, estop, CHANGE);
  
}//end setup



//Loop
/////////////////////////////////////////////Powerboard Loop Forever
void loop() 
{
  if( FromPowerboard.receiveData() )
  {   
    switch (receive_powerboard_command.data_id)
      { 
      case 0:
        break;
        
      case BMS_BATTERY_PACK_ON_OFF:
        digitalWrite(ESTOP_CNTRL_P6_0, LOW);
        break;
        
     // case BMS_BATTERY_PACK_ON_OFF_REBOOT:
        //break;
        
      default:
      //Serial.print("Unrecognized data :");
      //Serial.println(data);
      break; 
       }//endswitch
  }//end if
  
  if( estop_occured_flag )
  {
    send_powerboard_telem.data_id = BMS_BATTERY_PACK_ON_OFF;
    ToPowerboard.sendData();
    send_powerboard_telem.data_id = 0;
  }//end if
  
}//end loop


void estop()
{    
  // Keep from interrupt ourselves
  detachInterrupt(ESTOP_P5_5);
  
  digital_reading = digitalRead(ESTOP_P5_5);
  
  if(digital_reading == LOW)
  {
    // Turn off everything when estop
    digitalWrite(ESTOP_CNTRL_P6_0, LOW);
    
  }else{
    
    delay(ESTOP_DELAY_MILLIS);
    
    //debounce on estop on
    digital_reading = digitalRead(ESTOP_P5_5);
    
    if(digital_reading)
    { 
      // Turn off everything when estop
      digitalWrite(ESTOP_CNTRL_P6_0, HIGH);
      
     }//end if    
   }//end else 
       
   attachInterrupt(ESTOP_P5_5, estop, CHANGE); 
}//end functn



