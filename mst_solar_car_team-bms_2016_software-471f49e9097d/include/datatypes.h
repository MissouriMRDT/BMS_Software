/**
 * @file datatypes.h
 *
 * @date Mar 1, 2015
 * @author Jesse Cureton
 */

#ifndef DATATYPES_H_
#define DATATYPES_H_

//Data types
typedef signed char int8_t;				///< Signed 8-bit integer
typedef unsigned char uint8_t;			///< Unsigned 8-bit integer
typedef signed short int16_t;			///< Signed 16-bit integer
typedef unsigned short uint16_t;		///< Unsigned 16-bit integer
typedef signed long int32_t;			///< Signed 32-bit integer
typedef unsigned long uint32_t;			///< Unsigned 32-bit integer
typedef signed long long int64_t;		///< Signed 64-bit integer
typedef unsigned long long uint64_t;	///< Unsigned 64-bit integer

//Global constants
#define FALSE		0
#define TRUE		1

/** Temperature type storing temperature, address, and bank for all temperature sensors */
typedef struct temp_
{
	uint64_t addr;
	uint8_t  bank;
	float 	 temperature[6];	//AVERAGE_PERIOD
}temp_t;

/** Cell type storing voltage, resistance, bank, and cell */
typedef struct cell_
{
	float 	voltage[6];			//AVERAGE_PERIOD
	float 	resistance[6];		//AVERAGE_PERIOD
	uint8_t bank;
	uint8_t cell;
}cell_t;

//These typedefs are used to create a block of memory of a certain size (16, 32, and 64 bits) that can then be addressed
//as various different datatypes internally. Used primarily in CAN messages, which may need to contain several types of data.

/**
 * A 64-bit union of memory.
 *
 * Used primarily in {@link can.c}, this type can be used to create a 64 bit block of memory which can
 * then be addressed as any 64-bit combination of two float values, 8 signed/unsigned 8-bit values,
 * 4 signed/unsigned 16-bit values, 2 signed/unsigned 32-bit values, or 1 unsigned 64-bit value.
 *
 * Example:
 * <code><pre>
 * group_64 val;
 * val.data_fp[0] = (float) 42.000;
 * val.data_u16[2] = 0x4242;
 * val.data_u8[6] = 0x42;
 * val.data_u8[7] = 0x42;
 * </pre></code>
 *
 * @see {@link processCANMessages()} for an example of how this is used to add data of different types to a CAN message for transmission.
 */
typedef union _group_64 {
	float data_fp[2];
	uint8_t data_u8[8];
	int8_t data_8[8];
	uint16_t data_u16[4];
	int16_t data_16[4];
	uint32_t data_u32[2];
	int32_t data_32[2];
	uint64_t data_u64;
} group_64;

/**
 * A 32-bit union of memory.
 *
 * @see {@link group_64} for details on this implementation, as this is the same with only 32-bits of memory.
 */
typedef union _group_32 {
	float data_fp;
	unsigned char data_u8[4];
	char data_8[4];
	unsigned int data_u16[2];
	int data_16[2];
	unsigned long data_u32;
	long data_32;
} group_32;

/**
 * A 16-bit union of memory.
 *
 * @see {@link group_64} for details on this implementation, as this is the same with only 16 bits of memory.
 */
typedef union _group_16 {
	unsigned char data_u8[2];
	char data_8[2];
	unsigned int data_u16;
	int data_16;
} group_16;

/**
 * A CAN message.
 */
typedef struct _can_message {
	uint16_t status;	///< One of the {@link CAN_MSG_STATUS} values
	uint16_t address;	///< The address the message is to be transmitted on
	group_64 data;		///< The data to be sent with the message
}can_message;


#endif /* DATATYPES_H_ */
