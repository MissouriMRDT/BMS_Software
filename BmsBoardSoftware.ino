// Standard C
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// todo ? Task_sleep(DELAY);

// Pack Auto Kill
const int PACK_SOFT_FUSE_AMPS_MAX = 180;

//////////////////////////////////////////////RoveBoard
// Pins
const int ESTOP_WRITE_P6_0        = 2;

const int ESTOP_READ_P1_6         = 15;
const int PACK_AMPS_READ_P5_5     = 30;
const int PACK_VOLTS_READ_P5_3    = 61;

// SoftSPI using Adafruit Liquid Crystal product : ID 292
const int LCD_LATCH_P4_7       = 28;
const int LCD_DATA_P4_6        = 8;
const int LCD_CLOCK_P4_5       = 27;

// todo TLC6C5912-Q1
const int LED_GRAPH_CLEAR_P10_0           = 54; 
const int LED_GRAPH_DATA_SERIAL_P10_2     = 55; 
const int LED_GRAPH_LATCH_RCK_P10_1       = 70; 
const int LED_GRAPH_CLOCK_SRCK_P10_3      = 71; 

//////////////////////////////////////
// Timing
const int REBOOT_DELAY            = 5000;
const int REBOOT_TRY_COUNT        = 3;

const int DIGITAL_DEBOUNCE_DELAY  = 10;
const int ANALOG_DEBOUNCE_DELAY   = 10;
const int SERIAL_DELAY            = 10;
const int SPI_DELAY               = 10;

const int LOOP_DELAY              = 10;

/////////////////////////////////////////////RoveBoard
// Tiva1294C RoveBoard Specs
const float VCC                 = 3.3;       //volts
const float ADC_MAX             = 4096;      //bits
const float ADC_MIN             = 0;         //bits

//////////////////////////////////////////////Sensor
// ACS_722 IC Sensor Specs 
const float SENSOR_SENSITIVITY   = 0.0066;    //volts/amp
const float SENSOR_SCALE         = 0.5;       //volts/amp

const float SENSOR_BIAS          = VCC * SENSOR_SCALE;

const float AMPS_MAX             = (VCC - SENSOR_BIAS) / SENSOR_SENSITIVITY;
const float AMPS_MIN             = -SENSOR_BIAS / SENSOR_SENSITIVITY;

// todo
//max is ACTUALLY 3.054545 from the chip we should never see anything above 3.054545, but we need to account for 3.3 adc
const float PACK_ACTUAL_VOLTS_MAX = 33.6;
const float VOLTS_MAX            = (3.3/3.045454)*PACK_ACTUAL_VOLTS_MAX; 
const float VOLTS_MIN            = 0;

float amps_reading               = 0;
float volts_reading              = 0;

float adc_reading                = 0;
int   adc_threshhold             = 0;

float mapFloats(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}//end fnctn

////////////////////////////////////////////
// Highest pri task does pin setup?
void setupPackSoftEstop() 
{
  pinMode(ESTOP_WRITE_P6_0,    OUTPUT);
  pinMode(ESTOP_READ_P1_6,      INPUT);
  pinMode(PACK_AMPS_READ_P5_5,  INPUT);
  pinMode(PACK_VOLTS_READ_P5_3, INPUT);

  // Turn off everything when we begin
  digitalWrite(ESTOP_WRITE_P6_0, LOW);
}//end setup

/////////////////////////////////////////////
// Highest pri task?
void loopPackSoftEstop() 
{
  if( digitalRead(ESTOP_READ_P1_6) == HIGH)
  {  
    delay(DIGITAL_DEBOUNCE_DELAY);
     
    if( digitalRead(ESTOP_READ_P1_6) == HIGH )
    {
      digitalWrite(ESTOP_WRITE_P6_0, LOW);
    }//end if

  }else{

    delay(DIGITAL_DEBOUNCE_DELAY);    

    if( digitalRead(ESTOP_READ_P1_6) == LOW )
    {
      digitalWrite(ESTOP_WRITE_P6_0, HIGH);
    }//end if 

  }//end if 

  delay(LOOP_DELAY);  

}//end loop

