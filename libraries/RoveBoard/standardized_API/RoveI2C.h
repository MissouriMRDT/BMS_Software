#ifndef ROVEI2C_H_
#define ROVEI2C_H_

#include <stdint.h>
#include <stddef.h>

typedef enum RoveI2C_Speed
{
  //standard i2c speed, max of 100 kbit/s
  I2CSPEED_STANDARD,

  //full i2c speed, max of 400 kbit/s
  I2CSPEED_FULL,

  //fast-mode i2c speed, max of 1000 kbit/s
  I2CSPEED_FAST,

  //high speed-mode i2c speed, max of 3 Mbits/s
  I2CSPEED_HIGH

} RoveI2C_Speed;

typedef struct RoveI2C_Handle
{
  uint8_t index;
  bool initialized;
  uint16_t clockPin;
  uint16_t dataPin;
  RoveI2C_Speed speed;
#ifdef __cplusplus
  RoveI2C_Handle()
  {
    initialized = false;
  }
#endif
} RoveI2C_Handle;

typedef enum RoveI2C_Error
{
  //no errors encountered in exchange
  I2CERROR_NONE,

  //an acknowledgement wasn't received from the other device
  I2CERROR_ACK,

  //the other device held the clock line low past timeout
  I2CERROR_TIMEOUT,

  //the line was busy; in master mode, this means another master was talking
  I2CERROR_BUSY,

  //some other kind of error occurred
  I2CERROR_OTHER

} RoveI2C_Error;

//Initializes the i2c module for usage
//input: i2cIndex: the index of the module, based on board specific constatns
//       speed: The speed of communication. Acceptable inputs are board specific
//       clock and data pin: The SCL and SDA pins you want to use, that should match the module.
//returns: a handle for the now initialized i2c module
extern RoveI2C_Handle roveI2cInit(uint8_t i2cIndex, RoveI2C_Speed speed, uint8_t clockPin, uint8_t dataPin);

//sends a single message to a slave device, without specifying a destination register within the slave
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The message byte to transmit
//returns: result of the transmission
extern RoveI2C_Error roveI2cSend(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t msg);

//sends a single message to a slave device, specifying a destination register within the slave
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The address of the destination register within the slave device, from 0 to 255
//       The message byte to transmit
//returns: result of the transmission
extern RoveI2C_Error roveI2cSendReg(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t reg, uint8_t msg);

//sends a series of messages to a slave device, without specifying a destination register within the slave
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The message array to transmit
//       The amount of messages in the array
//returns: result of the transmission
extern RoveI2C_Error roveI2cSendBurst(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t msg[], size_t msgSize);

//sends a series of messages to a slave device, specifying the starting destination register within the slave
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The address of the starting destination register within the slave from 0 to 255
//       The message array to transmit
//       The amount of messages in the array
//returns: result of the transmission
extern RoveI2C_Error roveI2cSendBurstReg(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t reg, uint8_t msg[], size_t msgSize);

//requests a byte of information from a slave, without specifying a register within the slave to read from.
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The variable, passed by pointer, to store the received byte into
//returns: result of the transmission
extern RoveI2C_Error roveI2cReceive(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t* buffer);

//requests a byte of information from a slave, specifying a register within the slave to read from.
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The address of the slave register to read from
//       The variable, passed by pointer, to store the received byte into
//returns: result of the transmission
extern RoveI2C_Error roveI2cReceiveReg(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t reg, uint8_t* buffer);

//requests a series of bytes of information from a slave, without specifying a register within the slave to read from.
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The buffer to fill up with the received information
//       The amount of bytes to receive
//returns: result of the transmission
extern RoveI2C_Error roveI2cReceiveBurst(RoveI2C_Handle handle, uint16_t SlaveAddr, uint8_t* buffer, size_t sizeOfReceive);

//requests a series of bytes of information from a slave, specifying the starting register within the slave to first read from.
//input: The handle of the i2c module to use, gained from the init function
//       The address of the slave device
//       The address of the register within the slave to read from.
//       The buffer to fill up with the received information
//       The amount of bytes to receive
//returns: result of the transmission
extern RoveI2C_Error roveI2cReceiveBurstReg(RoveI2C_Handle handle, uint16_t SlaveAddr,  uint8_t reg, uint8_t* buffer, size_t sizeOfReceive);

#endif
