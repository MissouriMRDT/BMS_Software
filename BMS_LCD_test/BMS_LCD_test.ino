#include "BMS_LCD_test.h"

// Setup & Main Loop ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial2.begin(9600);
  startScreen();
  delay(300);

  setPinInputs();
  setPinOutputs();
  setPinOutputStates(); 
 // //Serial.println("Setup Complete.");
} //end setup



void setPinInputs()
{
    pinMode(CELL1_VOLTAGE_PIN,      INPUT);
    pinMode(CELL2_VOLTAGE_PIN,      INPUT);
    pinMode(CELL3_VOLTAGE_PIN,      INPUT);
    pinMode(CELL4_VOLTAGE_PIN,      INPUT);
    pinMode(CELL5_VOLTAGE_PIN,      INPUT);
    pinMode(CELL6_VOLTAGE_PIN,      INPUT);
    pinMode(CELL7_VOLTAGE_PIN,      INPUT);
    pinMode(CELL8_VOLTAGE_PIN,      INPUT);

    pinMode(TEMP_SENSE_PIN,         INPUT);
    pinMode(V_OUT_SENSE_PIN,        INPUT);
    pinMode(PACK_I_SENSE_PIN,       INPUT);
    pinMode(LOGIC_POWER_SENSE_PIN,  INPUT);
}

void setPinOutputs()
{
    pinMode(SW_IND_PIN,             OUTPUT);
    pinMode(SW_ERR_IND_PIN,         OUTPUT);
    pinMode(V_OUT_IND_PIN,          OUTPUT);
    pinMode(FANS_IND_PIN,           OUTPUT);

    pinMode(BUZZER_CONTROL_PIN,     OUTPUT);  
    pinMode(FAN_CONTROL_PIN,        OUTPUT);
    pinMode(PACK_GATE_PIN,           OUTPUT);
    pinMode(LCD_TX_PIN,             OUTPUT);
    pinMode(LOGIC_SWITCH_PIN,       OUTPUT);
}

void setPinOutputStates()
{
    digitalWrite(BUZZER_CONTROL_PIN,    LOW);
    digitalWrite(FAN_CONTROL_PIN,       LOW);
    digitalWrite(PACK_GATE_PIN,         HIGH);
    digitalWrite(LOGIC_SWITCH_PIN,      LOW);
    digitalWrite(SW_IND_PIN,            LOW);
    digitalWrite(SW_ERR_IND_PIN,        LOW);
    digitalWrite(V_OUT_IND_PIN,         LOW);
    digitalWrite(FANS_IND_PIN,          LOW);
}


void startScreen()
{
  
  // put your main code here, to run repeatedly: 
  delay(350);
  //Send the clear command to the display - this returns the cursor to the beginning of the display
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
  
  /*Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);*/
  
  
  //movingRover();
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
  for(int i = 0; i<4; i++)
  {
    asterisks();
    stars();
  }
  delay(10);

  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
}

void movingRover()
{
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
for(int i = 0; i<14;i++)
{
    delay(450);
   //delay(200);
    //Serial2.write(0x20);
   // Serial2.write(0x20);
   
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    
    Serial2.write(0xA1);
    
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    
      Serial2.write(0xDB);
      Serial2.write(0xBA);
      Serial2.write(0xDA);
      Serial2.write(0xCD);
   
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20); 
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
       
      Serial2.write(0x6F);
      Serial2.write(0x5E);
      Serial2.write(0x6F);
      Serial2.write(0x5E);
      Serial2.write(0x6F);
      
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
    Serial2.write(0x20);
  /*  
    Serial2.write(0x23);//#
    Serial2.write(0x52);//R
    Serial2.write(0x4F);//O
    Serial2.write(0x56);//V
    Serial2.write(0x45);//E
    Serial2.write(0x53);//S
    Serial2.write(0x4F);//O
    Serial2.write(0x48);//H
    Serial2.write(0x41);//A
    Serial2.write(0x52);//R
    Serial2.write(0x44);//D*/
  }
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
}

void asterisks()
{
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
  Serial2.write(0x56);//V
  Serial2.write(0x41);//A
  Serial2.write(0x4C);//L
  Serial2.write(0x4B);//K
  Serial2.write(0x59);//Y
    ////////////////////////////////////////////////////////////////need a y
  Serial2.write(0x52);//R
  Serial2.write(0x49);//I
  Serial2.write(0x45);//E
  Serial2.write(0x20);
  Serial2.write(0x49);//I
  Serial2.write(0x4E);//N
  Serial2.write(0x49);//I
  Serial2.write(0x54);//T
  Serial2.write(0x49);//I
  Serial2.write(0x41);//A
  Serial2.write(0x4C);//L
  Serial2.write(0x49);//I
  Serial2.write(0x5A);//Z
  Serial2.write(0x49);//I
  Serial2.write(0x4E);//N
  //Serial2.write(0x47);//G
 
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x20);
  
    Serial2.write(0xA1);
  
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
    Serial2.write(0xDB);
    Serial2.write(0xBA);
    Serial2.write(0xDA);
    Serial2.write(0xCD);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20); 
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);   
    Serial2.write(0x6F);
    Serial2.write(0x5E);
    Serial2.write(0x6F);
    Serial2.write(0x5E);
    Serial2.write(0x6F);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  //Serial2.write(0x2A);

  delay(500);
  
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
}

void stars()
{
  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
  Serial2.write(0x56);//V
  Serial2.write(0x41);//A
  Serial2.write(0x4C);//L
  Serial2.write(0x4B);//K
  Serial2.write(0x59);//Y
  ////////////////////////////////////////////////////////////////need a y
  Serial2.write(0x52);//R
  Serial2.write(0x49);//I
  Serial2.write(0x45);//E
  Serial2.write(0x20);
  Serial2.write(0x49);//I
  Serial2.write(0x4E);//N
  Serial2.write(0x49);//I
  Serial2.write(0x54);//T
  Serial2.write(0x49);//I
  Serial2.write(0x41);//A
  Serial2.write(0x4C);//L
  Serial2.write(0x49);//I
  Serial2.write(0x5A);//Z
  Serial2.write(0x49);//I
  Serial2.write(0x4E);//N
  //Serial2.write(0x47);//G
 
  Serial2.write(0x20);
  Serial2.write(0x2B);
  
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x20);
     Serial2.write(0xA1);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
    Serial2.write(0xDB);
    Serial2.write(0xBA);
    Serial2.write(0xDA);
    Serial2.write(0xCD);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20); 
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x2A);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  Serial2.write(0x20);
  Serial2.write(0x20);
  Serial2.write(0x20);   
    Serial2.write(0x6F);
    Serial2.write(0x5E);
    Serial2.write(0x6F);
    Serial2.write(0x5E);
    Serial2.write(0x6F);
  Serial2.write(0x20);
  Serial2.write(0x2B);
  //Serial2.write(0x2B);

  delay(500);

  Serial2.write('|'); //Setting character
  Serial2.write('-'); //Clear display
}