// RoveBoard.cpp for Energia
// Author: Gbenga Osibodu

#include <supportingUtilities/Debug.h>
#include "../Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "../RoveUart/HardwareSerial.h"
#include "RoveUart_TivaTm4c1294NCPDT.h"
#include "../RovePinMap_TivaTM4C1294NCPDT.h"

static HardwareSerial* uartArray[8] = {&Serial , &Serial1, &Serial2, &Serial3,
                                  &Serial4, &Serial5, &Serial6, &Serial7};

bool validateInput(unsigned int uart_index, unsigned int txPin, unsigned int rxPin)
{
  switch(uart_index)
  {
    case 0:
      if((txPin == PA_1 && rxPin == PA_0))
      {
        return true;
      }
      break;

    case 1:
      if((txPin == PB_1) && (rxPin == PB_0))
      {
        return true;
      }
      break;

    case 2:
      if((txPin == PA_7 || txPin == PD_5) && (rxPin == PA_6 || rxPin == PD_4))
      {
        return true;
      }
      break;

    case 3:
      if((txPin == PA_5 || txPin == PJ_1) && (rxPin == PA_4 || rxPin == PJ_0))
      {
        return true;
      }
      break;

    case 4:
      if((txPin == PK_1 || txPin == PA_3) && (rxPin == PK_0 || rxPin == PA_2))
      {
        return true;
      }
      break;

    case 5:
      if((txPin == PC_7 && rxPin == PC_6))
      {
        return true;
      }
      break;

    case 6:
      if((txPin == PP_1 && rxPin == PP_0))
      {
        return true;
      }
      break;

    case 7:
      if((txPin == PC_5 && rxPin == PC_4))
      {
        return true;
      }
      break;
  }

  return false;
}

RoveUart_Handle roveUartOpen(uint8_t uart_index, uint32_t baud_rate, uint8_t txPin, uint8_t rxPin)
{
  if(!validateInput(uart_index, txPin, rxPin))
  {
    debugFault("roveUartOpen: uart index or pins is nonsense");
  }

  uartArray[uart_index] -> begin(baud_rate, rxPin, txPin);
  RoveUart_Handle handle;
  handle.uart_index = uart_index;
  handle.initialized = true;

  return handle;
}

RoveUart_Error roveUartWrite(RoveUart_Handle uart, void* write_buffer, size_t bytes_to_write) {
  if(uart.initialized == false)
  {
    debugFault("roveUartWrite: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];
  serial -> write((uint8_t*)write_buffer, bytes_to_write);
  serial -> flush();
    
  return ROVE_UART_ERROR_SUCCESS;
}

int roveUartPeek(RoveUart_Handle uart)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartPeek: handle not initialized");
  }

	HardwareSerial* serial = uartArray[uart.uart_index];

	return serial -> peek();
}

int roveUartPeekIndex(RoveUart_Handle uart, uint16_t peekIndex)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartPeek: handle not initialized");
  }

  if(peekIndex == 0)
  {
    return roveUartPeek(uart);
  }
  else
  {
    HardwareSerial* serial = uartArray[uart.uart_index];
    return serial -> peek(peekIndex);
  }
}

RoveUart_Error roveUartRead(RoveUart_Handle uart, void* read_buffer, size_t bytes_to_read) {
  if(uart.initialized == false)
  {
    debugFault("roveUartRead: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  if (bytes_to_read == 0) {
    return ROVE_UART_ERROR_SUCCESS;
  }
  
  if (read_buffer == NULL) {
    return ROVE_UART_ERROR_UNKNOWN;
  }

  for (int i =0; i<bytes_to_read; i++) {
    while(serial -> available() == 0);
    ((unsigned char*)read_buffer)[i] = serial -> read();//Serial.println(temp[i],DEC);
  }
  
  return ROVE_UART_ERROR_SUCCESS;
} 

RoveUart_Error roveUartReadNonBlocking(RoveUart_Handle uart, void* read_buffer, size_t bytes_to_read)
{
	if(roveUartAvailable(uart) < bytes_to_read)
	{
		return ROVE_UART_ERROR_UNKNOWN;
	}
	else
	{
		return roveUartRead(uart, read_buffer, bytes_to_read);
	}
}

int roveUartAvailable(RoveUart_Handle uart) {
  if(uart.initialized == false)
  {
    debugFault("roveUartAvailable: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  return serial -> available();
}

RoveUart_Error roveUartSettings(RoveUart_Handle uart,unsigned int parityBits, unsigned int stopBits, unsigned int wordLength)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartSettings: handle not initialized");
  }

	HardwareSerial* serial = uartArray[uart.uart_index];

	serial -> setOutputSettings(parityBits, stopBits, wordLength);

	return ROVE_UART_ERROR_SUCCESS;
}

void roveUartSetBufferLength(RoveUart_Handle uart, uint16_t length)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartSetFIFOLength: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  serial->setBufferSize(length);
}

RoveUart_Error roveUartFlushAll(RoveUart_Handle uart)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartFlushAll: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  serial->flushAll();

  return ROVE_UART_ERROR_SUCCESS;
}

uint16_t roveUartGetBufferLength(RoveUart_Handle uart)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartGetFIFOLength: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  return serial->getBufferSize();
}

void roveUartAttachReceiveCb(void (*userFunc)(uint8_t moduleThatReceived))
{
  attachTransmitCb(userFunc);
}

void roveUartAttachTransmitCb(void (*userFunc)(uint8_t moduleThatTransmitted))
{
  attachReceiveCb(userFunc);
}
