/*
 ************************************************************************
 *	HardwareSerial.cpp
 *
 *	Arduino core files for ARM Cortex-M4F: Tiva-C and Stellaris
 *		Copyright (c) 2012 Robert Wessels. All right reserved.
 *
 *
 ***********************************************************************
 2013-12-23 Limited size for RX and TX buffers, by spirilis

  Derived from:
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 9/3/17 by drue satterfield, who took it wholesale for roveboard
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "../Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "HardwareSerial.h"
#include "../RovePinMap_TivaTM4C1294NCPDT.h"
#include "../tivaware/inc/hw_memmap.h"
#include "../tivaware/inc/hw_types.h"
#include "../tivaware/inc/hw_ints.h"
#include "../tivaware/inc/hw_uart.h"
#include "../tivaware/driverlib/gpio.h"
#include "../tivaware/driverlib/debug.h"
#include "../tivaware/driverlib/interrupt.h"
#include "../tivaware/driverlib/rom.h"
#include "../tivaware/driverlib/rom_map.h"
#include "../tivaware/driverlib/pin_map.h"
#include "../tivaware/driverlib/sysctl.h"
#include "../tivaware/driverlib/uart.h"

#define TX_BUFFER_EMPTY    (txReadIndex == txWriteIndex)
#define TX_BUFFER_FULL     (((txWriteIndex + 1) % txBufferSize) == txReadIndex)

#define RX_BUFFER_EMPTY    (rxReadIndex == rxWriteIndex)
#define RX_BUFFER_FULL     (((rxWriteIndex + 1) % rxBufferSize) == rxReadIndex)

#define UART_BASE g_ulUARTBase[uartModule]

static const uint8_t MaxCallbacks = 8;
static void (*receiveCbFuncs[MaxCallbacks])(uint8_t module);
static void (*transmitCbFuncs[MaxCallbacks])(uint8_t module);

static const unsigned long g_ulUARTBase[8] =
{
    UART0_BASE, UART1_BASE, UART2_BASE, UART3_BASE,
	UART4_BASE, UART5_BASE, UART6_BASE, UART7_BASE
};

//*****************************************************************************
//
// The list of possible interrupts for the console UART.
//
//*****************************************************************************
static const unsigned long g_ulUARTInt[8] =
{
    INT_UART0, INT_UART1, INT_UART2, INT_UART3,
	INT_UART4, INT_UART5, INT_UART6, INT_UART7
};

//*****************************************************************************
//
// The list of UART peripherals.
//
//*****************************************************************************
static const unsigned long g_ulUARTPeriph[8] =
{
    SYSCTL_PERIPH_UART0, SYSCTL_PERIPH_UART1, SYSCTL_PERIPH_UART2,
	SYSCTL_PERIPH_UART3, SYSCTL_PERIPH_UART4, SYSCTL_PERIPH_UART5,
	SYSCTL_PERIPH_UART6, SYSCTL_PERIPH_UART7
};
//*****************************************************************************
//
// The list of UART GPIO configurations.
//
//*****************************************************************************
static const uint32_t g_ulUARTConfig[] = {
    0,          // dummy
    0,          // 01 - 3.3v       X8_01
    0,            // 02 - PE_4       X8_03
    GPIO_PC4_U7RX,            // 03 - PC_4       X8_05
    GPIO_PC5_U7TX,            // 04 - PC_5       X8_07
    GPIO_PC6_U5RX,            // 05 - PC_6       X8_09
    0,            // 06 - PE_5       X8_11
    0,            // 07 - PD_3       X8_13
    GPIO_PC7_U5TX,            // 08 - PC_7       X8_15
    0,            // 09 - PB_2       X8_17
    0,            // 10 - PB_3       X8_19
    0,            // 11 - PP_2       X9_20
    0,            // 12 - PN_3       X9_18
    0,            // 13 - PN_2       X9_16
    0,            // 14 - PD_0       X9_14
    0,            // 15 - PD_1       X9_12
    0,          // 16 - RST        X9_10
    0,            // 17 - PH_3       X9_08
    0,            // 18 - PH_2       X9_06
    0,            // 19 - PM_3       X9_04
    0,          // 20 - GND        X9_02
    0,          // 21 - 5v         X8_02
    0,          // 22 - GND        X8_04
    0,            // 23 - PE_0       X8_06
    0,            // 24 - PE_1       X8_08
    0,            // 25 - PE_2       X8_10
    0,            // 26 - PE_3       X8_12
    0,            // 27 - PD_7       X8_14
    GPIO_PA6_U2RX,            // 28 - PA_6       X8_16
    0,            // 29 - PM_4       X8_18
    0,            // 30 - PM_5       X8_20
    0,            // 31 - PL_3       X9_19
    0,            // 32 - PL_2       X9_17
    0,            // 33 - PL_1       X9_15
    0,            // 34 - PL_0       X9_13
    0,            // 35 - PL_5       X9_11
    0,            // 36 - PL_4       X9_09
    0,            // 37 - PG_0       X9_07
    0,            // 38 - PF_3       X9_05
    0,            // 39 - PF_2       X9_03
    0,            // 40 - PF_1       X9_01
    0,          // 41 - 3.3v       X6_01
    0,            // 42 - PD_2       X6_03
    GPIO_PP0_U6RX,            // 43 - PP_0       X6_05
    GPIO_PP1_U6TX,            // 44 - PP_1       X6_07
    GPIO_PD4_U2RX,            // 45 - PD_4       X6_09
    GPIO_PD5_U2TX,            // 46 - PD_5       X6_11
    0,            // 47 - PQ_0       X6_13
    0,            // 48 - PP_4       X6_15
    0,            // 49 - PN_5       X6_17
    0,            // 50 - PN_4       X6_19
    0,            // 51 - PM_6       X7_20
    0,            // 52 - PQ_1       X7_18
    0,            // 53 - PP_3       X7_16
    0,            // 54 - PQ_3       X7_14
    0,            // 55 - PQ_2       X7_12
    0,          // 56 - RESET      X7_10
    GPIO_PA7_U2TX,            // 57 - PA_7       X7_08
    0,            // 58 - PP_5       X7_06
    0,            // 59 - PM_7       X7_04
    0,          // 6Z - GND        X7_02
    0,          // 61 - 5v         X6_02
    0,          // 62 - GND        X6_04
    0,            // 63 - PB_4       X6_06
    0,            // 64 - PB_5       X6_08
    GPIO_PK0_U4RX,            // 65 - PK_0       X6_10
    GPIO_PK1_U4TX,            // 66 - PK_1       X6_12
    0,            // 67 - PK_2       X6_14
    0,            // 68 - PK_3       X6_16
    GPIO_PA4_U3RX,            // 69 - PA_4       X6_18
    GPIO_PA5_U3TX,            // 70 - PA_5       X6_20
    0,            // 71 - PK_7       X7_19
    0,            // 72 - PK_6       X7_17
    0,            // 73 - PH_1       X7_15
    0,            // 74 - PH_0       X7_13
    0,            // 75 - PM_2       X7_11
    0,            // 76 - PM_1       X7_09
    0,            // 77 - PM_0       X7_07
    0,            // 78 - PK_5       X7_05
    0,            // 79 - PK_4       X7_03
    0,            // 80 - PG_1       X7_01
    0,            // 81 - PN_1       LED1
    0,            // 82 - PN_0       LED2
    0,            // 83 - PF_4       LED3
    0,            // 84 - PF_0       LED4
    GPIO_PJ0_U3RX,            // 85 - PJ_0       USR_SW1
    GPIO_PJ1_U3TX,            // 86 - PJ_1       USR_SW2
    0,            // 87 - PD_6       AIN5
    GPIO_PA0_U0RX,            // 88 - PA_0       JP4
    GPIO_PA1_U0TX,            // 89 - PA_1       JP5
    GPIO_PA2_U4RX,            // 90 - PA_2       X11_06
    GPIO_PA3_U4TX,            // 91 - PA_3       X11_08
    0,            // 92 - PL_6       unrouted
    0,            // 93 - PL_7       unrouted
    GPIO_PB0_U1RX,            // 94 - PB_0       X11_58
    GPIO_PB1_U1TX,            // 95 - PB_1       unrouted
};

// Constructors ////////////////////////////////////////////////////////////////
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
}
// Private Methods //////////////////////////////////////////////////////////////
void
HardwareSerial::flushAll(void)
{
    // wait for transmission of outgoing data
    while(!TX_BUFFER_EMPTY)
    {
    }
    while (UARTBusy(UART_BASE)) ;
    txReadIndex = 0;
    txWriteIndex = 0;

    //
    // Flush the receive buffer.
    //
    rxReadIndex = 0;
    rxWriteIndex = 0;
}

void
HardwareSerial::primeTransmit(unsigned long ulBase)
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
        IntDisable(g_ulUARTInt[uartModule]);
        //
        // Yes - take some characters out of the transmit buffer and feed
        // them to the UART transmit FIFO.
        //
        while(!TX_BUFFER_EMPTY)
        {
            while(UARTSpaceAvail(ulBase) && !TX_BUFFER_EMPTY){
                UARTCharPutNonBlocking(ulBase,
                                       txBuffer[txReadIndex]);

                txReadIndex = (txReadIndex + 1) % txBufferSize;
            }
        }

        //
        // Reenable the UART interrupt.
        //
        IntEnable(g_ulUARTInt[uartModule]);
    }
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::setOutputSettings(uint8_t paritySettings, uint8_t stopBitSettings, uint8_t wordLengthSettings)
{
	uint32_t wordLength, stopBits, parityBits;
	switch(paritySettings)
	{
		case NoParity:
			parityBits = UART_CONFIG_PAR_NONE;
			break;

		case EvenParity:
			parityBits = UART_CONFIG_PAR_EVEN;
			break;

		case OddParity:
			parityBits = UART_CONFIG_PAR_ODD;
			break;

		case AlwaysZero:
			parityBits = UART_CONFIG_PAR_ZERO;
			break;

		case AlwaysOne:
			parityBits = UART_CONFIG_PAR_ONE;
			break;

		default:
			return;
	}

	switch(stopBitSettings)
	{
		case OneStopBit:
			stopBits = UART_CONFIG_STOP_ONE;
			break;

		case TwoStopBit:
			stopBits = UART_CONFIG_STOP_TWO;
			break;

		default:
			return;
	}

	switch(wordLengthSettings)
	{
		case WordLength8:
			wordLength = UART_CONFIG_WLEN_8;
			break;

		case WordLength7:
			wordLength = UART_CONFIG_WLEN_7;
			break;

		case WordLength6:
			wordLength = UART_CONFIG_WLEN_6;
			break;

		case WordLength5:
			wordLength = UART_CONFIG_WLEN_5;
			break;

		default:
			return;
	}

	//function call automatically stops uart for configuration
	UARTConfigSetExpClk(UART_BASE, getCpuClockFreq(), baudRate, (wordLength | parityBits |stopBits));
}

void
HardwareSerial::begin(unsigned long baud, unsigned int rxPin, unsigned int txPin)
{
	baudRate = baud;

  //
  // Initialize the UART.
  //
  SysCtlPeripheralEnable(g_ulUARTPeriph[uartModule]);

  GPIOPinConfigure(g_ulUARTConfig[txPin]);
  GPIOPinConfigure(g_ulUARTConfig[rxPin]);

  GPIOPinTypeUART(portRefToPortBase[pinToPortRef[txPin]], pinToPinMask[txPin] | pinToPinMask[rxPin]);

  UARTConfigSetExpClk(UART_BASE, getCpuClockFreq(), baudRate,
                          (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                           UART_CONFIG_WLEN_8));

  switch(UART_BASE)
  {
    case UART0_BASE:
      UARTIntRegister(UART_BASE, UARTIntHandler0);
    break;

    case UART1_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler1);
    break;

    case UART2_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler2);
    break;

    case UART3_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler3);
    break;

    case UART4_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler4);
    break;

    case UART5_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler5);
    break;

    case UART6_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler6);
    break;

    case UART7_BASE:
    UARTIntRegister(UART_BASE, UARTIntHandler7);
    break;
  }

  //
  // Set the UART to interrupt whenever a byte is transmitted or received
  //
  UARTFIFOLevelSet(UART_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
  flushAll();
  UARTIntDisable(UART_BASE, 0xFFFFFFFF);
  UARTIntEnable(UART_BASE, UART_INT_RX | UART_INT_RT);
  IntEnable(g_ulUARTInt[uartModule]);

  //
  // Enable the UART operation.
  //
  UARTEnable(UART_BASE);

  if (txBuffer != (unsigned char *)0xFFFFFFFF)  // Catch attempts to re-init this Serial instance by freeing old buffer first
    delete txBuffer;
  if (rxBuffer != (unsigned char *)0xFFFFFFFF)  // Catch attempts to re-init this Serial instance by freeing old buffer first
    delete rxBuffer;
  txBuffer = new unsigned char [txBufferSize];
  rxBuffer = new unsigned char [rxBufferSize];

  SysCtlDelay(100);
}

void
HardwareSerial::setBufferSize(unsigned long buffSize)
{
  if(buffSize == 0)
  {
    return;
  }

  unsigned char* oldRxBuffer = rxBuffer;
  unsigned char* oldTxBuffer = txBuffer;
  unsigned long oldBuffSize = txBufferSize;

  IntDisable(g_ulUARTInt[uartModule]);

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

  IntEnable(g_ulUARTInt[uartModule]);
}

unsigned long HardwareSerial::getBufferSize()
{
  return txBufferSize; //rx, tx share same buff size
}

HardwareSerial::operator bool()
{
	return true;
}

void HardwareSerial::end()
{
    unsigned long ulInt = IntMasterDisable();

	flushAll();

    //
    // If interrupts were enabled when we turned them off, turn them
    // back on again.
    //
    if(!ulInt)
    {
        IntMasterEnable();
    }

    IntDisable(g_ulUARTInt[uartModule]);
    UARTIntDisable(UART_BASE, UART_INT_RX | UART_INT_RT);
}

int HardwareSerial::available(void)
{
    return((rxWriteIndex >= rxReadIndex) ?
		(rxWriteIndex - rxReadIndex) : rxBufferSize - (rxReadIndex - rxWriteIndex));
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

int HardwareSerial::peek(void)
{
    unsigned char cChar = 0;

    //
    // Wait for a character to be received.
    //
    if(RX_BUFFER_EMPTY)
    {
    	return -1;
    	//
        // Block waiting for a character to be received (if the buffer is
        // currently empty).
        //
    }

    //
    // Read a character from the buffer.
    //
    cChar = rxBuffer[rxReadIndex];
    //
    // Return the character to the caller.
    //
    return(cChar);
}

int HardwareSerial::read(void)
{
    if(RX_BUFFER_EMPTY) {
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
    while(!TX_BUFFER_EMPTY);
    while (UARTBusy(UART_BASE)) ;
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--) {
	n += write(*buffer++);
  }
  return n;
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

    unsigned int numTransmit = 0;
    //
    // Check for valid arguments.
    //
    ASSERT(c != 0);

    //
    // Send the character to the UART output.
    //
    while (TX_BUFFER_FULL);
    txBuffer[txWriteIndex] = c;
    txWriteIndex = (txWriteIndex + 1) % txBufferSize;
    numTransmit ++;

    //
    // If we have anything in the buffer, make sure that the UART is set
    // up to transmit it.
    //
    if(!TX_BUFFER_EMPTY)
    {
	    primeTransmit(UART_BASE);
      UARTIntEnable(UART_BASE, UART_INT_TX);
    }

    //
    // Return the number of characters written.
    //
    return(numTransmit);
}

void HardwareSerial::UARTIntHandler(void){
    unsigned long ulInts;
    long lChar;
    // Get and clear the current interrupt source(s)
    //
    ulInts = UARTIntStatus(UART_BASE, true);
    UARTIntClear(UART_BASE, ulInts);

    // Are we being interrupted because the TX FIFO has space available?
    //
    if(ulInts & UART_INT_TX)
    {
        //
        // Move as many bytes as we can into the transmit FIFO.
        //
        primeTransmit(UART_BASE);

        //
        // If the output buffer is empty, turn off the transmit interrupt. Run any user transmit callbacks
        //
        if(TX_BUFFER_EMPTY)
        {
            UARTIntDisable(UART_BASE, UART_INT_TX);
            uint8_t i;
            for(i = 0; i < MaxCallbacks; i++)
            {
              if(transmitCbFuncs[i])
              {
                transmitCbFuncs[i](uartModule);
              }
            }
        }
    }
    if(ulInts & (UART_INT_RX | UART_INT_RT))
    {
        while(UARTCharsAvail(UART_BASE))
        {

            //
            // Read a character
            //
            lChar = UARTCharGetNonBlocking(UART_BASE);
            //
            // If there is space in the receive buffer, put the character
            // there, otherwise throw it away.
            //
            uint8_t volatile full = RX_BUFFER_FULL;
            if(full) break;

            rxBuffer[rxWriteIndex] =
                (unsigned char)(lChar & 0xFF);
            rxWriteIndex = ((rxWriteIndex) + 1) % rxBufferSize;

            //
            // call user receive callbacks
            //
            uint8_t i;
            for(i = 0; i < MaxCallbacks; i++)
            {
              if(receiveCbFuncs[i])
              {
                receiveCbFuncs[i](uartModule);
              }
            }

        }
        primeTransmit(UART_BASE);
        UARTIntEnable(UART_BASE, UART_INT_TX);
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

void
UARTIntHandler4(void)
{
    Serial4.UARTIntHandler();
}

void
UARTIntHandler5(void)
{
    Serial5.UARTIntHandler();
}

void
UARTIntHandler6(void)
{
    Serial6.UARTIntHandler();
}

void
UARTIntHandler7(void)
{
    Serial7.UARTIntHandler();
}

HardwareSerial Serial;
HardwareSerial Serial1(1);
HardwareSerial Serial2(2);
HardwareSerial Serial3(3);
HardwareSerial Serial4(4);
HardwareSerial Serial5(5);
HardwareSerial Serial6(6);
HardwareSerial Serial7(7);
