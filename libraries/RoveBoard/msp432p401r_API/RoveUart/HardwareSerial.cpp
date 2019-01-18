/*
 * HardwareSerial.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: drue
 *      Taken from energia framework
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "HardwareSerial.h"
#include "../Clocking/Clocking_MSP432P401R.h"
#include "../msp432Ware/inc/msp.h"
#include "../RovePinMap_MSP432P401R.h"

#define TX_BUFFER_EMPTY    (txReadIndex == txWriteIndex)
#define TX_BUFFER_FULL     (((txWriteIndex + 1) % txBufferSize) == txReadIndex)

#define RX_BUFFER_EMPTY    (rxReadIndex == rxWriteIndex)
#define RX_BUFFER_FULL     (((rxWriteIndex + 1) % rxBufferSize) == rxReadIndex)

#define UART_CLOCK_FREQ    CS_getSMCLK()
#define UART_CLOCK_SOURCE  EUSCI_A_UART_CLOCKSOURCE_SMCLK

#define UART_INTERRUPTS    EUSCI_A_UART_TRANSMIT_INTERRUPT | EUSCI_A_UART_RECEIVE_INTERRUPT

static const uint8_t MaxCallbacks = 3;
static void (*receiveCbFuncs[MaxCallbacks])(uint8_t module);
static void (*transmitCbFuncs[MaxCallbacks])(uint8_t module);

#define UART_BASE g_ulUARTBase[uartModule]

static const unsigned long g_ulUARTBase[] =
{
  EUSCI_A0_BASE, EUSCI_A1_BASE, EUSCI_A2_BASE, EUSCI_A3_BASE
};

/////////// Constructors ////////////////////////////////////////////////////////////////
HardwareSerial::HardwareSerial(void)
{
  txWriteIndex = 0;
  txReadIndex = 0;
  rxWriteIndex = 0;
  rxReadIndex = 0;
  uartModule = 0;

  txBuffer = (unsigned char *) 0xFFFFFFFF;
  rxBuffer = (unsigned char *) 0xFFFFFFFF;
  txBufferSize = SERIAL_BUFFER_SIZE;
  rxBufferSize = SERIAL_BUFFER_SIZE;

  transmitting = false;
}

HardwareSerial::HardwareSerial(unsigned long module)
{
  txWriteIndex = 0;
  txReadIndex = 0;
  rxWriteIndex = 0;
  rxReadIndex = 0;
  uartModule = module;

  txBuffer = (unsigned char *) 0xFFFFFFFF;
  rxBuffer = (unsigned char *) 0xFFFFFFFF;
  txBufferSize = SERIAL_BUFFER_SIZE;
  rxBufferSize = SERIAL_BUFFER_SIZE;

  transmitting = false;
}

// Private Methods //////////////////////////////////////////////////////////////
void
HardwareSerial::flushAll(void)
{
  flush();

  //
  // reset the buffers.
  //
  rxReadIndex = 0;
  rxWriteIndex = 0;
  txReadIndex = 0;
  txWriteIndex = 0;
}

void
HardwareSerial::primeTransmit()
{
  //
  // Do we have any data to transmit?
  //
  if(!TX_BUFFER_EMPTY)
  {
    //
    // Disable the UART interrupt. If we don't do this there is a race
    // condition which can cause the read index to be corrupted.
    //
    UART_disableInterrupt(UART_BASE, UART_INTERRUPTS);

    //
    // Yes - take some characters out of the transmit buffer and feed
    // them to the UART transmit FIFO.
    //
    //UART_transmitData(UART_BASE, txBuffer[txReadIndex]); for some reason the driverlib function fails
    EUSCI_A_CMSIS(UART_BASE)->TXBUF = txBuffer[txReadIndex];
    txReadIndex = (txReadIndex + 1) % txBufferSize;

    //
    // Reenable the UART interrupt.
    //
    UART_enableInterrupt(UART_BASE, UART_INTERRUPTS);

    transmitting = true;
  }
  else
  {
    transmitting = false;
  }
}

// Public Methods //////////////////////////////////////////////////////////////
void HardwareSerial::setOutputSettings(uint8_t paritySettings, uint8_t stopBitSettings)
{
  switch(paritySettings)
  {
    case NoParity:
      config.parity = EUSCI_A_UART_NO_PARITY;
      break;

    case EvenParity:
      config.parity = EUSCI_A_UART_EVEN_PARITY;
      break;

    case OddParity:
      config.parity = EUSCI_A_UART_ODD_PARITY;
      break;
  }

  switch(stopBitSettings)
  {
    case OneStopBit:
      config.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
      break;

    case TwoStopBit:
      config.numberofStopBits = EUSCI_A_UART_TWO_STOP_BITS;
      break;
  }
  UART_disableModule(UART_BASE);
  UART_initModule(UART_BASE, &config);

  switch(UART_BASE)
  {
    case EUSCI_A0_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler0);
      break;

    case EUSCI_A1_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler1);
      break;

    case EUSCI_A2_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler2);
      break;
  case EUSCI_A3_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler3);
      break;
  }

  //
  // Set the UART to interrupt whenever a character is transmitted or when any character is received.
  //
  flushAll();
  UART_enableInterrupt(UART_BASE, UART_INTERRUPTS);

  //
  // Enable the UART operation.
  //
  UART_enableModule(UART_BASE);
}

void
HardwareSerial::begin(unsigned long baud, uint8_t txPin, uint8_t rxPin)
{
  //set pins
  GPIO_setAsPeripheralModuleFunctionInputPin(pinToPinPort[txPin], pinToPinMask[txPin], GPIO_PRIMARY_MODULE_FUNCTION);
  GPIO_setAsPeripheralModuleFunctionInputPin(pinToPinPort[rxPin], pinToPinMask[rxPin], GPIO_PRIMARY_MODULE_FUNCTION);

  //setup uart module
  configureForBaud(baud);
  UART_initModule(UART_BASE, &config);

  switch(UART_BASE)
  {
    case EUSCI_A0_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler0);
      break;

    case EUSCI_A1_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler1);
      break;

    case EUSCI_A2_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler2);
      break;
  case EUSCI_A3_BASE:
      UART_registerInterrupt(UART_BASE, UARTIntHandler3);
      break;
  }

  // Catch attempts to re-init this Serial instance by freeing old buffer first
  if (txBuffer != (unsigned char *)0xFFFFFFFF || rxBuffer != (unsigned char *)0xFFFFFFFF)
  {
    delete txBuffer;
    delete rxBuffer;
  }

  txBuffer = new unsigned char [txBufferSize];
  rxBuffer = new unsigned char [rxBufferSize];

  //
  // Set the UART to interrupt whenever a character is transmitted or when any character is received.
  //
  flushAll();
  UART_enableInterrupt(UART_BASE, UART_INTERRUPTS);

  //
  // Enable the UART operation.
  //
  UART_enableModule(UART_BASE);

  int i;
  for(i = 0; i < 100; i++);
}

void HardwareSerial::setBufferSize(unsigned long buffSize)
{
  if(buffSize == 0)
  {
    return;
  }
  buffSize++; //we can actually hold buffsize - 1 bytes, since the ringbuffer stops when the indexes are one short of each other

  unsigned char* oldRxBuffer = rxBuffer;
  unsigned char* oldTxBuffer = txBuffer;
  unsigned long oldBuffSize = txBufferSize;

  UART_disableInterrupt(UART_BASE, UART_INTERRUPTS);

  txBuffer = new unsigned char [buffSize];
  rxBuffer = new unsigned char [buffSize];

  unsigned long i;
  for(i = 0; i < oldBuffSize; i++)
  {
    if(i >= buffSize)
    {
      break;
    }

    rxBuffer[i] = oldRxBuffer[i];
    txBuffer[i] = oldTxBuffer[i];
  }

  txBufferSize = buffSize;
  rxBufferSize = buffSize;
  delete oldRxBuffer;
  delete oldTxBuffer;

  UART_enableInterrupt(UART_BASE, UART_INTERRUPTS);
}

unsigned long HardwareSerial::getBufferSize()
{
  //we can actually hold buffsize - 1 bytes, since the ringbuffer stops when the indexes are one short of each other
  return txBufferSize - 1; //rx, tx share same buff size
}

void HardwareSerial::end()
{
  unsigned long ulInt = Interrupt_disableMaster();

  flushAll();

  //
  // If interrupts were enabled when we turned them off, turn them
  // back on again.
  //
  if(!ulInt)
  {
    Interrupt_enableMaster();
  }

  UART_disableInterrupt(UART_BASE, UART_INTERRUPTS);

  UART_disableModule(UART_BASE);
}

int HardwareSerial::available(void)
{
  return((rxWriteIndex >= rxReadIndex) ?
  (rxWriteIndex - rxReadIndex) : rxBufferSize - (rxReadIndex - rxWriteIndex));
}

int HardwareSerial::peek(void)
{
  //
  // If there isn't anything in the buffer, then don't block, return -1.
  //
  if(RX_BUFFER_EMPTY)
  {
    return -1;
  }

  //
  // Read a character from the buffer. Return it to the caller.
  //
  return rxBuffer[rxReadIndex];
}

int HardwareSerial::peek(uint16_t index)
{
  uint8_t cChar;

  //
  // Check to see if there's anything to get
  //
  if(RX_BUFFER_EMPTY || index >= available())
  {
    return -1;
  }

  uint16_t tempRead = ((rxReadIndex) + index) % rxBufferSize;

  //
  // Read a character from the buffer.
  //
  cChar = rxBuffer[tempRead];

  //
  // Return the character to the caller.
  //
  return(cChar);
}

int HardwareSerial::read(void)
{
  if(RX_BUFFER_EMPTY)
  {
    return -1;
  }

  //
  // Read a character from the buffer.
  //
  unsigned char cChar = rxBuffer[rxReadIndex];
  rxReadIndex = ((rxReadIndex) + 1) % rxBufferSize;
  return cChar;
}

void HardwareSerial::flush()
{
  //if we're not transmitting but tx buff isn't empty, then transmit until it is
  if(!transmitting && !TX_BUFFER_EMPTY)
  {
    primeTransmit();
  }
  while(transmitting);
  while (UART_queryStatusFlags(UART_BASE, EUSCI_A_UART_BUSY));
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size)
{
  size_t packetSize;
  int i;

  //
  // Determine how many bytes are going to be written out
  //
  if(size == 0)
  {
    return 0;
  }
  else if(size >= txBufferSize - 1) //can only fill up 16 of the 17 spaces, as the 17th represents the txRead starting point
  {
    packetSize = txBufferSize - 1;
  }
  else
  {
    packetSize = size;
  }

  //
  // Load up the tx buffer
  //
  while (!TX_BUFFER_EMPTY); //wait for any previous transmissions to finish

  for(i = 0; i < packetSize; i++) //load the packets into the transmission buffer
  {
    txBuffer[txWriteIndex] = buffer[i];
    txWriteIndex = (txWriteIndex + 1) % txBufferSize;
  }

  //
  // Begin transmission. Function call will begin transmission and transmit the first byte, then interrupts will take care of the rest.
  //
  primeTransmit();


  return size;
}

size_t HardwareSerial::println(const char* str)
{
  int i = 0;
  size_t cnt = 0;

  while(str[i] != 0)
  {
    write(str[i]);
    cnt++;
  }

  return cnt;
}

size_t HardwareSerial::write(uint8_t c)
{
  //
  // Send the character to the UART output.
  //
  while (!TX_BUFFER_EMPTY); //wait for any previous transmissions to finish
  txBuffer[txWriteIndex] = c;
  txWriteIndex = (txWriteIndex + 1) % txBufferSize;

  primeTransmit();

  //
  // Return the number of characters written.
  //
  return(1);
}

void HardwareSerial::UARTIntHandler(void)
{
  uint32_t ulInts;
  uint8_t lChar;

  //
  // Get and clear the current interrupt source(s)
  //
  ulInts = UART_getInterruptStatus(UART_BASE, UART_INTERRUPTS);
  UART_clearInterruptFlag(UART_BASE, ulInts);

  // Are we being interrupted because the TX FIFO has space available?
  //
  if(ulInts & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
  {
    //
    // If the output buffer is empty, turn off the transmit interrupt. It will get re-enabled
    // when the user calls another write function
    //
    if(TX_BUFFER_EMPTY)
    {
      UART_disableInterrupt(UART_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
      transmitting = false;

      //
      // Transmit is finished, to run user transmit callback functions
      //
      uint8_t i;
      for(i = 0; i < MaxCallbacks; i++)
      {
        if(transmitCbFuncs[i])
        {
          transmitCbFuncs[i](uartModule);
        }
      }
    }

    //
    // Transmit another byte
    //
    else
    {
      primeTransmit();
    }
  }

  //Did we receive something?
  if(ulInts & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
  {
    //
    // Read a character
    //
    lChar = UART_receiveData(UART_BASE);

    //
    // If there is space in the receive buffer, put the character
    // there, otherwise throw it away. Run user receive callbacks
    //
    uint8_t volatile full = RX_BUFFER_FULL;
    if(!full)
    {
      rxBuffer[rxWriteIndex] = lChar;
      rxWriteIndex = ((rxWriteIndex) + 1) % rxBufferSize;

      uint8_t i;
      for(i = 0; i < MaxCallbacks; i++)
      {
        if(receiveCbFuncs[i])
        {
          receiveCbFuncs[i](uartModule);
        }
      }
    }
  }
}

void HardwareSerial::configureForBaud(uint32_t baud)
{
  //all logic taken from msp432 datasheet, uart section.
  //http://www.ti.com/lit/ug/slau356f/slau356f.pdf

  uint32_t fUart = UART_CLOCK_FREQ; //note: assumed that baud < uart clock freq
  uint16_t N;
  float temp;

  config.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
  config.parity = EUSCI_A_UART_NO_PARITY;
  config.selectClockSource = UART_CLOCK_SOURCE;
  config.uartMode = EUSCI_A_UART_MODE;
  config.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;

  N = fUart / baud;
  config.secondModReg = lookupSecondModReg(N);

  if(N > 16) //gotta oversample it
  {
    config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
    config.clockPrescalar = (uint16_t)(N/16);
    temp = (float)(N/16) - (uint16_t)(N/16);
    config.firstModReg = (uint8_t)(temp * 16);
  }
  else
  {
    config.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
    config.clockPrescalar = N;
    config.firstModReg = 0;
  }
}

uint8_t HardwareSerial::lookupSecondModReg(float N)
{
  //http://www.ti.com/lit/ug/slau356f/slau356f.pdf
  //if you want to see where all these nonsense magic numbers came from, uart baud rate section

  float fractionN = (float)((float)(N) - (uint16_t)(N));
  if(fractionN < 0) //should never happen, but hey
  {
    fractionN *= -1;
  }

  if(fractionN < .0529)
  {
    return 0x00;
  }
  else if(fractionN < .0715)
  {
    return 0x01;
  }
  else if(fractionN < .0835)
  {
    return 0x02;
  }
  else if(fractionN < .1001)
  {
    return 0x04;
  }
  else if(fractionN < .1252)
  {
    return 0x08;
  }
  else if(fractionN < .1430)
  {
    return 0x10;
  }
  else if(fractionN < .1670)
  {
    return 0x20;
  }
  else if(fractionN < .2147)
  {
    return 0x11;
  }
  else if(fractionN < .2224)
  {
    return 0x21;
  }
  else if(fractionN < .2503)
  {
    return 0x22;
  }
  else if(fractionN < .3000)
  {
    return 0x44;
  }
  else if(fractionN < .3335)
  {
    return 0x25;
  }
  else if(fractionN < .3575)
  {
    return 0x49;
  }
  else if(fractionN < .3753)
  {
    return 0x4A;
  }
  else if(fractionN < .4003)
  {
    return 0x52;
  }
  else if(fractionN < .4286)
  {
    return 0x92;
  }
  else if(fractionN < .4378)
  {
    return 0x53;
  }
  else if(fractionN < .5002)
  {
    return 0x55;
  }
  else if(fractionN < .5715)
  {
    return 0xAA;
  }
  else if(fractionN < .6003)
  {
    return 0x6B;
  }
  else if(fractionN < .6254)
  {
    return 0xAD;
  }
  else if(fractionN < .6432)
  {
    return 0xB5;
  }
  else if(fractionN < .6667)
  {
    return 0xB6;
  }
  else if(fractionN < .7001)
  {
    return 0xD6;
  }
  else if(fractionN < .7147)
  {
    return 0xB7;
  }
  else if(fractionN < .7503)
  {
    return 0xBB;
  }
  else if(fractionN < .7861)
  {
    return 0xDD;
  }
  else if(fractionN < .8004)
  {
    return 0xED;
  }
  else if(fractionN < .8333)
  {
    return 0xEE;
  }
  else if(fractionN < .8464)
  {
    return 0xBF;
  }
  else if(fractionN < .8572)
  {
    return 0xDF;
  }
  else if(fractionN < .8751)
  {
    return 0xEF;
  }
  else if(fractionN < .9004)
  {
    return 0xF7;
  }
  else if(fractionN < .9170)
  {
    return 0xFB;
  }
  else if(fractionN < .9288)
  {
    return 0xFD;
  }
  else
  {
    return 0xFE;
  }
}

void attachTransmitCb(void (*userFunc)(uint8_t))
{
  uint8_t i;
  for(i = 0; i < MaxCallbacks; i++)
  {
    if(!transmitCbFuncs[i])
    {
      transmitCbFuncs[i] = userFunc;
      break;
    }
  }
}

void attachReceiveCb(void (*userFunc)(uint8_t))
{
  uint8_t i;
  for(i = 0; i < MaxCallbacks; i++)
  {
    if(!receiveCbFuncs[i])
    {
      receiveCbFuncs[i] = userFunc;
      break;
    }
  }
}

void
UARTIntHandler0(void)
{
  Serial.UARTIntHandler();
}

void
UARTIntHandler1(void)
{
  Serial1.UARTIntHandler();
}

void
UARTIntHandler2(void)
{
  Serial2.UARTIntHandler();
}

void
UARTIntHandler3(void)
{
  Serial3.UARTIntHandler();
}


HardwareSerial Serial;
HardwareSerial Serial1(1);
HardwareSerial Serial2(2);
HardwareSerial Serial3(3);
