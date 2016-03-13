//P6.0
#define EstopWritePin 2
//P5.5
#define EstopReadPin 30

#define ESTOP_THRESHOLD 1000

int EstopReadValue = 0;

float voltage = 0;

int ctrl = 0;

// the setup routine runs once when you press reset
void setup() 
{ 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
 
  pinMode(EstopWritePin, OUTPUT);
  pinMode(EstopReadPin, INPUT);
  
  digitalWrite(EstopWritePin, LOW);
}//end setup

void loop() 
{
  
   if(Serial.available() >0 )
  {
    ctrl = Serial.parseInt();
  }//end if
  
  switch (ctrl) 
  {
    case 0:
      digitalWrite(EstopWritePin, LOW);
      break;
      
    case 1:
      digitalWrite(EstopWritePin, HIGH);
      break;
 
  }//end switch
  
  analogRead(EstopReadValue);
  
  Serial.print(" EstopReadValue: "); 
  Serial.print(EstopReadValue);
  
  voltage = EstopReadValue / 4096 * 3.3;
  Serial.print(" : Converted to Voltage: "); 
  Serial.println(voltage, DEC);
  
  if(EstopReadValue > ESTOP_THRESHOLD )
  {
    digitalWrite(EstopWritePin, HIGH);
  }//end if
  
}//end loop

