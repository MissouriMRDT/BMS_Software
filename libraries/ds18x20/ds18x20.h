#ifndef DS18X20_H_
#define DS18X20_H_

#define DS1820_OUT 					P2OUT
#define DS1820_DIR 					P2DIR
#define DS1820_SEL					P2SEL
#define DS1820_IN					P2IN
#define DS1820_DATA_IN_PIN          BIT4
#define DS1820_VCC			        BIT3
#define DS1820_GND          		BIT1



#define DS1820_SKIP_ROM             0xCC
#define DS1820_READ_SCRATCHPAD      0xBE
#define DS1820_CONVERT_T            0x44

void InitDS18B20(void);
unsigned int ResetDS1820 ( void );
void DS1820_HI(void);
void DS1820_LO(void);
void WriteZero(void);
void WriteOne(void);
unsigned int ReadBit(void);
void WriteDS1820 (unsigned char,int );
unsigned int ReadDS1820 ( void );
float GetData(void);
#endif /*DS18X20_H_*/
