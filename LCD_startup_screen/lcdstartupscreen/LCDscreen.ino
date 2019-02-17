



void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600); //Start serial communication at 9600

Serial7.begin(9600); //Start communication with Serial7
/*Serial7.write('|');
Serial7.write(128 + 29);
Serial7.write('|');
Serial7.write(158+0);
Serial7.write('|');
Serial7.write(188+0);*/
 /* Serial7.write('|');
Serial7.write(24);
Serial7.write(120);*/


}

void loop() {


  // put your main code here, to run repeatedly: 
  delay(1000);
  //Send the clear command to the display - this returns the cursor to the beginning of the display
  Serial7.write('|'); //Setting character
  Serial7.write('-'); //Clear display
  
  Serial7.write(0x20);
  Serial7.write(0x20);
  
  movingRover();
   Serial7.write('|'); //Setting character
  Serial7.write('-'); //Clear display
  for(int i = 0; i<4; i++)
  {
    asterisks();
    stars();
  }
  delay(1000);

  Serial7.write('|'); //Setting character
  Serial7.write('-'); //Clear display
}

void movingRover()
{
for(int i = 0; i<14;i++)
{
    delay(450);
   // Serial7.write(0x20);
   // Serial7.write(0x20);
   
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    
    Serial7.write(0xA1);
    
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    
      Serial7.write(0xDB);
      Serial7.write(0xBA);
      Serial7.write(0xDA);
      Serial7.write(0xCD);
   
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20); 
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
       
      Serial7.write(0x6F);
      Serial7.write(0x5E);
      Serial7.write(0x6F);
      Serial7.write(0x5E);
      Serial7.write(0x6F);
      
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
    Serial7.write(0x20);
  /*  
    Serial7.write(0x23);//#
    Serial7.write(0x52);//R
    Serial7.write(0x4F);//O
    Serial7.write(0x56);//V
    Serial7.write(0x45);//E
    Serial7.write(0x53);//S
    Serial7.write(0x4F);//O
    Serial7.write(0x48);//H
    Serial7.write(0x41);//A
    Serial7.write(0x52);//R
    Serial7.write(0x44);//D*/
  }
  Serial7.write('|'); //Setting character
  Serial7.write('-'); //Clear display
}

void asterisks()
{
  Serial7.write(0x56);//V
  Serial7.write(0x41);//A
  Serial7.write(0x4C);//L
  Serial7.write(0x4B);//K
  Serial7.write(0x52);//R
  Serial7.write(0x49);//I
  Serial7.write(0x45);//E
  Serial7.write(0x20);
  Serial7.write(0x49);//I
  Serial7.write(0x4E);//N
  Serial7.write(0x49);//I
  Serial7.write(0x54);//T
  Serial7.write(0x49);//I
  Serial7.write(0x41);//A
  Serial7.write(0x4C);//L
  Serial7.write(0x49);//I
  Serial7.write(0x5A);//Z
  Serial7.write(0x49);//I
  Serial7.write(0x4E);//N
  Serial7.write(0x47);//G
 
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
    Serial7.write(0xA1);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
    Serial7.write(0xDB);
    Serial7.write(0xBA);
    Serial7.write(0xDA);
    Serial7.write(0xCD);
  Serial7.write(0x20);
  Serial7.write(0x20);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20); 
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  Serial7.write(0x20);
  Serial7.write(0x20);
  Serial7.write(0x20);   
    Serial7.write(0x6F);
    Serial7.write(0x5E);
    Serial7.write(0x6F);
    Serial7.write(0x5E);
    Serial7.write(0x6F);
  Serial7.write(0x20);
  Serial7.write(0x2A);
  //Serial7.write(0x2A);

  delay(500);
  
  Serial7.write('|'); //Setting character
  Serial7.write('-'); //Clear display
}

void stars()
{
   Serial7.write(0x56);//V
  Serial7.write(0x41);//A
  Serial7.write(0x4C);//L
  Serial7.write(0x4B);//K
  Serial7.write(0x52);//R
  Serial7.write(0x49);//I
  Serial7.write(0x45);//E
  Serial7.write(0x20);
  Serial7.write(0x49);//I
  Serial7.write(0x4E);//N
  Serial7.write(0x49);//I
  Serial7.write(0x54);//T
  Serial7.write(0x49);//I
  Serial7.write(0x41);//A
  Serial7.write(0x4C);//L
  Serial7.write(0x49);//I
  Serial7.write(0x5A);//Z
  Serial7.write(0x49);//I
  Serial7.write(0x4E);//N
  Serial7.write(0x47);//G
 
  Serial7.write(0x20);
  Serial7.write(0x2B);
  
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
    Serial7.write(0xA1);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
    Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
    Serial7.write(0xDB);
    Serial7.write(0xBA);
    Serial7.write(0xDA);
    Serial7.write(0xCD);
  Serial7.write(0x20);
  Serial7.write(0x20);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20); 
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  Serial7.write(0x20);
  Serial7.write(0x20);
  Serial7.write(0x20);   
    Serial7.write(0x6F);
    Serial7.write(0x5E);
    Serial7.write(0x6F);
    Serial7.write(0x5E);
    Serial7.write(0x6F);
  Serial7.write(0x20);
  Serial7.write(0x2B);
  //Serial7.write(0x2B);

  delay(500);

  Serial7.write('|'); //Setting character
  Serial7.write('-'); //Clear display
}
