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
#include <SPI.h>



//Testing
//
//////////////////////////////////////////////Debug Flags
const int ECHO_SERIAL_MONITOR_DEBUG =     1;
const int DELAY_SERIAL_MILLIS_DEBUG =     100;
const int POWERBOARD_SERIAL_DEBUG   =     0;



//Developing 
//
// Todo: Cameron/Mike Hardware Calibration
//
// Todo: Connor/Reed Edit
//
//digital
const int ESTOP_DELAY_MILLIS  = 250; 
volatile int digital_reading  = HIGH;

//analog
const int AUTO_KILL_THRESHHOLD_AMPS = 19;
const int ANALOG_DEBOUNCE_TIME_MILLIS = 10;



//Hardware
//
// Todo Mike and Cameron sign off
//
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



// the setup routine runs once when you press reset
void setup() 
{   
  // initialize serial communication at 9600 bits per second:
  if(ECHO_SERIAL_MONITOR_DEBUG)
  {
    Serial.begin(9600);
  }//end if
  
  if(POWERBOARD_SERIAL_DEBUG)
  { 
    Serial1.begin(9600);
  }// end if
  
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
  bool fuse_did_trip = checkSoftwareFuse(MIKE_TODO);
  
  if( checkSoftwareFuse(MIKE_TODO) )
  {
    digitalWrite(ESTOP_CNTRL_P6_0, LOW);
  }//end if
    
  if(POWERBOARD_SERIAL_DEBUG)
  { 
    Serial1.print("checkSoftwareFuse : ");
    Serial1.println(fuse_did_trip);
  }// end if
  
}//end loop



//Developing
///////////////////////////////////////////////Implementation
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
   
  /////////////////////////////////////////////Serial Monitor
  if(ECHO_SERIAL_MONITOR_DEBUG)
  {     
    Serial.print("ESTOP_P5_5 digital_reading: "); 
    Serial.print(digital_reading, DEC);
    delay(DELAY_SERIAL_MILLIS_DEBUG);
  }//end if
      
   attachInterrupt(ESTOP_P5_5, estop, CHANGE); 
}//end functn



//Developing
///////////////////////////////////////////////Implementation
bool checkSoftwareFuse(int bouncing_pin)
{  
  float adc_reading = analogRead(bouncing_pin);
  float current_reading = mapFloats(adc_reading, ADC_MIN, ADC_MAX, CURRENT_MIN, CURRENT_MAX); 
  
  if(current_reading < AUTO_KILL_THRESHHOLD_AMPS)
  {
    return false;
  }//else
  
  unsigned long system_time_micros = micros(); 

  while( system_time_micros != ( micros()  + ANALOG_DEBOUNCE_TIME_MILLIS)  )
  {
    adc_reading = analogRead(bouncing_pin);
    current_reading = mapFloats(adc_reading, ADC_MIN, ADC_MAX, CURRENT_MIN, CURRENT_MAX);
    
    if(current_reading < AUTO_KILL_THRESHHOLD_AMPS)
    {
      return false;
    }//end if
  }//end if 
    
  return true;
}//end functn

///////////////////////////////////////////////Implementation
float mapFloats(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}//end fnctn

