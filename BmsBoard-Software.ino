//RoveWare Bmsboard LT Todo?? Interface
//
// Judah jrs6w7
//
// Using Todo??
//
// Standard C
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



//Testing
//
//////////////////////////////////////////////Debug Flags
const int RED_COMMS_DEBUG =               0;
const int ECHO_SERIAL_MONITOR_DEBUG =     1;
const int DELAY_SERIAL_MILLIS_DEBUG =     100;



//Hardware
//
// Todo Mike and Cameron sign off
//
//////////////////////////////////////////////RoveBoard
// Tiva1294C RoveBoard Specs
const float VCC                 = 3.3;       //volts
const float ADC_MAX             = 4096;      //bits
const float ADC_MIN             = 0;         //bits

float adc_reading = 0;


const int ESTOP_CNTRL_P6_0    = 2;
const int ESTOP_P5_5          = 30;



//Platform
//////////////////////////////////////////////Energia
// Energia libraries used by RoveWare itself
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

//////////////////////////////////////////////Roveware
#include "RoveEthernet.h"
#include "RoveComm.h"

// RED udp device id by fourth octet
const int BMS_BOARD_IP_DEVICE_ID   = 51;

// RED can toggle the bus by bool
const uint16_t NO_ROVECOMM_MESSAGE   = 0;
const uint16_t BUS_5V_ON_OFF         = 207;

//Rovecomm :: RED packet :: data_id and data_value with number of data bytes size
uint16_t data_id       = 0;
size_t   data_size     = 0; 
uint16_t data_value    = 0;



//Developing 
//
// Todo: Cameron/Mike Hardware Calibration
//
// Todo: Connor/Reed Edit
//

//digital
int digitalDebounce(int bouncing_pin);
const int DIGITAL_DEBOUNCE_TIME_MICROS = 250;
const int DIGITAL_TRY_COUNT = 250; 

//analog
int analogDebounce(int bouncing_pin);
const int ANALOG_ACCEPTABLE_DRIFT = 250;
const int ANALOG_DEBOUNCE_TIME_MICROS = 250;
const int ANALOG_TRY_COUNT = 250;

//error
const int PIN_TOO_NOISY = -1;



//Begin
//
// Todo Mike and Cameron sign off
//
//////////////////////////////////////////////Powerboard Begin
// the setup routine runs once when you press reset
void setup() 
{  
  // Control Pins are outputs
  pinMode(ESTOP_CNTRL_P6_0, OUTPUT);

  // Turn on everything when we begin
  digitalWrite(ESTOP_CNTRL_P6_0, HIGH);
  
  // initialize serial communication at 9600 bits per second:
  if(ECHO_SERIAL_MONITOR_DEBUG)
  {
    Serial.begin(9600);
  }//end if
  
  if(RED_COMMS_DEBUG)
  { 
    roveComm_Begin(192, 168, 1, BMS_BOARD_IP_DEVICE_ID);
  }// end if
}//end setup



//Loop
//
// Todo Reed and Connor sign off
//
/////////////////////////////////////////////Powerboard Loop Forever
void loop() 
{
  /////////////////////////////////////////////Serial Monitor
  if(ECHO_SERIAL_MONITOR_DEBUG)
  {
    int digital_reading = digitalRead(ESTOP_P5_5);      
    Serial.print("ESTOP_P5_5 analogRead: "); 
    Serial.print(digital_reading, DEC);
    
    int adc_reading = analogRead(ESTOP_P5_5);      
    Serial.print(": ESTOP_P5_5 analogRead: "); 
    Serial.println(adc_reading, DEC);
    
    digital_reading = digitalDebounce(ESTOP_P5_5);      
    Serial.print("ESTOP_P5_5 digitalDebounce: "); 
    Serial.print(digital_reading, DEC);
    
    adc_reading = analogDebounce(ESTOP_P5_5);      
    Serial.print(": ESTOP_P5_5 analogDebounce: "); 
    Serial.println(adc_reading, DEC);
    
    delay(DELAY_SERIAL_MILLIS_DEBUG);
  }//end if
  
}//end loop



//Developing
///////////////////////////////////////////////Implementation
int digitalDebounce(int bouncing_pin)
{    
  // Count the bounces
  int digital_trend_count = 0;
  bool digital_reading = digitalRead(bouncing_pin);  
  
  // Read a bouncing pin and save the state
  bool last_digital_reading = digital_reading;   
  
  // Get timestamp from the system clock counter
  unsigned long system_time_micros = micros(); 
 
 // Spin for a max of millisec
  while(system_time_micros != ( micros()  + DIGITAL_DEBOUNCE_TIME_MICROS) )
  {
    digital_reading = digitalRead(bouncing_pin);
    
    if(digital_reading == last_digital_reading)
    {
      digital_trend_count++;
    }//end if
    
    if( (digital_reading != last_digital_reading) && (digital_trend_count > 0) )
    {
       digital_trend_count--; 
       last_digital_reading = digital_reading;
    }//end if
  
    if(digital_trend_count > DIGITAL_TRY_COUNT)
    {   
      
      return digital_reading;   
    }else{         
      
      last_digital_reading = digital_reading;
    }//end else
  }//end while
  
  return PIN_TOO_NOISY;
}//end functn



//Developing
///////////////////////////////////////////////Implementation
int analogDebounce(int bouncing_pin)
{    
  // Count the bounces
  int analog_trend_count = 0;
  bool analog_reading = analogRead(bouncing_pin);  
  
  // Read a bouncing pin and save the state
  bool last_analog_reading = analog_reading;   
  
  // Get timestamp from the system clock counter
  unsigned long system_time_micros = micros(); 
 
 // Spin for a max of millisec
  while(system_time_micros != ( micros()  + ANALOG_DEBOUNCE_TIME_MICROS) )
  {
    analog_reading = analogRead(bouncing_pin);
    
    if( analog_trend_count && (abs(analog_reading - last_analog_reading) < ANALOG_ACCEPTABLE_DRIFT)  )    
    {
      analog_trend_count++;
    }//end if
    
    if( analog_trend_count && (abs(analog_reading - last_analog_reading) > ANALOG_ACCEPTABLE_DRIFT)  )    
    {
       analog_trend_count--; 
       last_analog_reading = last_analog_reading;
    }//end if
  
    if(analog_trend_count > ANALOG_TRY_COUNT)
    {   
      
      return analog_reading;   
    }else{         
      
      last_analog_reading = analog_reading;
    }//end else
  }//end while
  
  return PIN_TOO_NOISY;
}//end functn











