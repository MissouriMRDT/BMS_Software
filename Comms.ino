  
//Energia
#include <Wire.h>

#include <EasyTransfer.h>

#include "Adafruit_LiquidCrystal.h"

// Data_id's for RED IP, passed through PowerBoard by serial
const uint8_t PACK_OFF             = 1040;
const uint8_t PACK_OFF_REBOOT      = 1041;

const uint8_t PACK_VOLTS           = 1072;
const uint8_t PACK_AMPS            = 1073;

// recieves commands from PowerBoard
typedef struct
{
  int16_t data_id;
  
} serial_rx;

// sends telem to PowerBoard
typedef struct
{
  int16_t data_id;
  float   data;
  
} serial_tx;

// data instance
serial_rx powerboard_command_rx;
serial_tx powerboard_telem_tx;

// Serial library instance
EasyTransfer FromPowerboard;
EasyTransfer ToPowerboard;

// LCD SoftSpi library instance
Adafruit_LiquidCrystal lcd(LCD_DATA, LCD_CLOCK, LCD_LATCH);

uint8_t led_bars = 0;

void setBarGraph_LED(int data_pin, int clock_pin, int latch_pin, uint8_t bar_graph, uint8_t bit_order = MSBFIRST, uint32_t delay_micros = 0)
{
  // latch low so the LED's don't change during shift
  digitalWrite(latch_pin, LOW);
    
  // shift out the bit
  shiftOut(data_pin, clock_pin, MSBFIRST, bar_graph);  

  // latch high so LED's light up  
  digitalWrite(latch_pin, HIGH);
  
  // pause before next value:
  delayMicroseconds(delay_micros);
}//end fnctn
    
uint8_t voltsToBarGraph_LED(float volts_reading)
{
  /*
uint8_t LED_bar_graph[8];
bar_graph_LED[0] = 0xFF; //11111111
bar_graph_LED[1] = 0xFE; //11111110
bar_graph_LED[2] = 0xFC; //11111100
bar_graph_LED[3] = 0xF8; //11111000
bar_graph_LED[4] = 0xF0; //11110000
bar_graph_LED[5] = 0xE0; //11100000
bar_graph_LED[6] = 0xC0; //11000000
bar_graph_LED[7] = 0x80; //10000000
 
bar_graph_LED[0] = 0xFF; //11111111
bar_graph_LED[1] = 0x7F; //01111111
bar_graph_LED[2] = 0x3F; //00111111
bar_graph_LED[3] = 0x1F; //00011111
bar_graph_LED[4] = 0x0F; //00001111
bar_graph_LED[5] = 0x07; //00000111
bar_graph_LED[6] = 0x03; //00000011
bar_graph_LED[7] = 0x01; //00000001
*/
  return -1;
}//end fnctn
   
/////////////////
// Lowest pri task?
void setupComms() 
{
  Serial.begin(9600);
  
  // bind library and and data
  FromPowerboard.begin(details(powerboard_command_rx), &Serial); 
  ToPowerboard.begin(  details(powerboard_telem_tx),   &Serial);

  //16 rows x 2 columns
  lcd.begin(16, 2);
  lcd.print("Zenith Battery Begin");
}//end setup

// Lowest pri task?
void loopComms() 
{
  if( FromPowerboard.receiveData() )
  {   
    switch (powerboard_command_rx.data_id)
      { 
      case 0:
        break;
      
      //No reboot shut down forever  
      case PACK_OFF:
      
        digitalWrite(ESTOP_WRITE_P6_0, LOW);
        break;
        
      //Single reboot shut down and reset
      case PACK_OFF_REBOOT:
      
        digitalWrite(ESTOP_WRITE_P6_0, LOW);
        delay(REBOOT_DELAY);
        
        digitalWrite(ESTOP_WRITE_P6_0, HIGH);
        break; 
        
      default:
      //Serial.print("Unrecognized data :");
      //Serial.println(data);
      break; 
      }//endswitch
  }//end if
  
  adc_reading = analogRead(PACK_AMPS_READ_P5_5);
  amps_reading = mapFloats(adc_reading, ADC_MIN, ADC_MAX, AMPS_MIN, AMPS_MAX); 
  
  powerboard_telem_tx.data_id = PACK_AMPS;
  powerboard_telem_tx.data = amps_reading;
  
  lcd.print("Amps : ");
  lcd.print(amps_reading);
  delay(SPI_DELAY);
  
  ToPowerboard.sendData(); 
  delay(SERIAL_DELAY); 
   
  // todo 
  //adc_reading = analogRead(PACK_VOLTS_READ_P5_3);
  //volts_reading = mapFloats(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, VOLTS_MAX); 
  //powerboard_telem_tx.data_id = PACK_VOLTS;
  //powerboard_telem_tx.data = volts_reading; 
  //ToPowerboard.sendData();  
  //delay(SERIAL_DELAY);
  
  led_bars = voltsToBarGraph_LED(volts_reading); 
  setBarGraph_LED(LED_GRAPH_DATA_SERIAL_P10_2, LED_GRAPH_CLOCK_SRCK_P10_3, LED_GRAPH_LATCH_RCK_P10_1, led_bars);
  
  delay(LOOP_DELAY);  
}//end loop

