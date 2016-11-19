/*//////////////////////////////////////////////////////////////////////
@file:            BMSSoftware.h
@author:        Emily "Ellis" Sansone
@description:    Assigns pin numbers to I/O variables, global constants, 
//////////////////////////////////////////////////////////////////////*/

///Pins
//named the same as in schematic, for clarity
const uint8_t PACK_GATE       =26;
const uint8_t PACK_I_MEAS     =2;
const uint8_t V_CHECK_ARRAY   =25;
const uint8_t V_CHECK_OUT     =27;
const uint8_t LOGIC_SWITCH    =24;
const uint8_t BMS_CSBI        =29;
const uint8_t BMS_MISO        =9;
const uint8_t BMS_MOSI        =10;
const uint8_t BMS_SCLK        =30;
const uint8_t SER_TX_IND      =15;
const uint8_t SER_RX_IND      =14;
const uint8_t SER_TX_PB       =4;
const uint8_t SER_RX_PB       =3;
const uint8_t FAN_CTRL_1      =19;
const uint8_t FAN_CTRL_2      =40;
const uint8_t FAN_CTRL_3      =39;
const uint8_t FAN_CTRL_4      =38;
const uint8_t BUZZER          =28;
const uint8_t LED_RCK         =59;
const uint8_t LED_CLR         =42;
const uint8_t LED_SRCK        =58;
const uint8_t GAUGE_ON        =57;
const uint8_t LED_SER_IN      =41;
const uint8_t TEMP_1          =18;
const uint8_t TEMP_2          =37;
const uint8_t LED1=78;
///Global Constants                  hold on here we go
//Timing
const uint16_t REBOOT_DELAY              = 5000;
const uint32_t IDLE_DELAY                = 3600000; //the rover sits idle for 1 hour before turning the BMS logic off.
const uint16_t REBOOT_TRY_COUNT          = 3;
const uint16_t DEBOUNCE_DELAY            = 10;
const uint16_t SERIAL_DELAY              = 10;
const uint16_t SPI_DELAY                 = 10;
// MSP432 RoveBoard Specs
const float VCC            = 3.3;       //volts
const float ADC_MAX        = 4096;      //bits
const float ADC_MIN        = 0;         //bits
const uint16_t LOOP_DELAY  = 10;         //mS
//ACS_759 IC Sensor Specs 
const float SENSOR_SENSITIVITY     = 0.0088;    //volts/amp
const float SENSOR_BIAS            = 0.0; //V. for now- determine empirically later
const float AMPS_MAX               = (VCC - SENSOR_BIAS) / SENSOR_SENSITIVITY; //amps
const float AMPS_MIN               = -SENSOR_BIAS / SENSOR_SENSITIVITY;        //amps
const uint16_t AMP_OVERCURRENT     = 180; //amps
//Voltage reading
// max is ACTUALLY 3.054545 from the chip we should never see anything above 3.054545, but we need to account for 3.3 adc
const float PACK_ACTUAL_VOLTS_MAX     = 33.6;
const float VOLTS_MAX                 = (3.3/3.045454)*PACK_ACTUAL_VOLTS_MAX; 
const float VOLTS_MIN                 = 0;
//const uint16_t BATTERY_LOW            = 2.7*8;  //V  warning low voltage
const uint16_t BATTERY_LOW_CRIT       = 2.5*8;  //V  low voltage threshold for shutoff to protect pack
//Various variables
uint16_t bars_LED = 0;
///Communication with Power Board
// Data_id's for RED IP, passed through PowerBoard by serial
const uint16_t PACK_OFF               =1040;
const uint16_t PACK_OFF_REBOOT        =1041;
const uint16_t CELL_1_VOLT            =1056;
const uint16_t CELL_2_VOLT            =1057;
const uint16_t CELL_3_VOLT            =1058;
const uint16_t CELL_4_VOLT            =1059;
const uint16_t CELL_5_VOLT            =1060;
const uint16_t CELL_6_VOLT            =1061;
const uint16_t CELL_7_VOLT            =1062;
const uint16_t CELL_8_VOLT            =1063;
const uint16_t PACK_VOLTS             =1072;
const uint16_t PACK_AMPS              =1073; 
const uint16_t TEMPERATURE_1                 =1074;
const uint16_t TEMPERATURE_2                 =1075;

// recieves commands from PowerBoard
struct serial_rx
{
  int16_t data_id;
};
// sends telem to PowerBoard
struct serial_tx
{
  int16_t data_id;
  float   data;
};

// data instance
serial_rx powerboard_command_rx;
serial_tx powerboard_telem_tx;
// Serial library instance
EasyTransfer FromPowerboard;
EasyTransfer ToPowerboard;
/*
OneWire oneWire(TEMP_1);
DallasTemperature tempSensors(&oneWire);
*/

