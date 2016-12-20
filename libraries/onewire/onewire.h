/**
 * @file onewire.h
 *
 * @date Mar 7, 2015
 * @author Jesse Cureton
 * @modified for MRDT by Emily "Ellis" Sansone, 12/10/2016
 */

 #include <stdint.h>
 
#ifndef ONEWIRE_H_
#define ONEWIRE_H_

/* we don't have these
//Macros for enable/disable the strong pullup
#define BANK1_LOW 	P1OUT &= ~TEMP1_MOSFET		///< Enable the strong pull-up MOSFET for BANK1
#define BANK1_HIGH 	P1OUT |= TEMP1_MOSFET		///< Disable the strong pull-up MOSFET for BANK1
#define BANK2_LOW 	P1OUT &= ~TEMP2_MOSFET		///< Enable the strong pull-up MOSFET for BANK2
#define BANK2_HIGH 	P1OUT |= TEMP2_MOSFET		///< Disable the strong pull-up MOSFET for BANK2
#define BANK3_LOW 	P1OUT &= ~TEMP3_MOSFET		///< Enable the strong pull-up MOSFET for BANK3
#define BANK3_HIGH 	P1OUT |= TEMP3_MOSFET		///< Disable the strong pull-up MOSFET for BANK3
*/
//Global search state
uint64_t curROMAddr;				///< Location where a proper 64-bit ROM address is stored after a successful 1-Wire search.
uint8_t	 LastDeviceFlag;
uint8_t  LastDiscrepancy;
uint8_t	 LastFamilyDiscrepancy;
uint8_t	 crc8;

//Primitive operations
void owWriteOne(uint8_t sensor);
void owWriteZero(uint8_t sensor);
uint8_t owReset(uint8_t sensor);
uint8_t owReadBit(uint8_t sensor);

//Byte level operations
void owWriteByte(uint8_t sensor, uint8_t byte);
uint16_t owReadByte(uint8_t sensor);

//Search operations
uint8_t owFindFirst(uint8_t sensor);
uint8_t owFindNext(uint8_t sensor);
uint8_t owSearch(uint8_t sensor);

//Delays in microseconds required for various things per the 1-Wire protocol
#define OW_SCTSPEED
#ifdef OW_STANDARD
	//Standard speed
	#define DELAY_A	6
	#define DELAY_B	64
	#define DELAY_C	60
	#define DELAY_D	10
	#define DELAY_E	9
	#define DELAY_F	55
	#define DELAY_G	0
	#define DELAY_H	480
	#define DELAY_I	70
	#define DELAY_J	410
#endif
#ifdef OW_SLOW
	//Standard speed
	#define DELAY_A	12
	#define DELAY_B	128
	#define DELAY_C	120
	#define DELAY_D	20
	#define DELAY_E	18
	#define DELAY_F	110
	#define DELAY_G	0
	#define DELAY_H	960
	#define DELAY_I	140
	#define DELAY_J	820
#endif
#ifdef OW_SCTSPEED
	//Standard speed is too slow. Custom SCT values for faster comms
	#define DELAY_A	4
	#define DELAY_B	42
	#define DELAY_C	40
	#define DELAY_D	6.5
	#define DELAY_E	6
	#define DELAY_F	36
	#define DELAY_G	0
	#define DELAY_H	320
	#define DELAY_I	46
	#define DELAY_J	270
#endif
#ifdef OW_OVERDRIVE
	//Overdrive speeds
	#define DELAY_A	1.0
	#define DELAY_B	7.5
	#define DELAY_C	7.5
	#define DELAY_D	2.5
	#define DELAY_E	1.0
	#define DELAY_F	7
	#define DELAY_G	2.5
	#define DELAY_H	70
	#define DELAY_I	8.5
	#define DELAY_J	40
#endif


#endif /* ONEWIRE_H_ */
