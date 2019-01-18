/* Programmer: Drue Satterfield
 * Date of creation: October 2017
 * Microcontroller used: MSP432P401R
 * Hardware components used by this file: all Uart modules, 0 to 3
 *
 *
 * Description: This library is used to implement basic Serial communication via the hardware UART modules.
 * Standard functions are implemented for receiving and transmitting series's of bytes, and also nonstandard for
 * changing the parity settings and the amount of stop bits per transmission.
 * Uart0 is wired to talk to the debugger on a computer instead of external devices. The rest use GPIO pins.
 * Those pins are:
 *
 * Module 1 RX: P2_2
 * Module 1 TX: P2_3
 * Module 2 RX: P3_2
 * Module 2 TX: P3_3
 * Module 3 RX: P9_6
 * Module 3 TX: P9_7
 *
 * Warnings:
 * This library uses 34 bytes of dynamic memory per uart initialized
 *
 */

#ifndef ROVEBOARD_MSP432P401R_API_ROVEUART_ROVEUART_MSP432P401R_H_
#define ROVEBOARD_MSP432P401R_API_ROVEUART_ROVEUART_MSP432P401R_H_

#include "../../standardized_API/RoveUart.h"

//roveUartSettings arguments:

//constants to set stop bits in transmission
#define OneStopBit 0
#define TwoStopBit 1

//constants to set up parity bits in transmission
#define NoParity 0
#define EvenParity 1
#define OddParity 2

//uart index arguments
#define uartModule0 0
#define uartModule1 1
#define uartModule2 2
#define uartModule3 3

//sets up the specified uart to run at the specified baud rate
//inputs: index of the uart module to run (0 for uart 0, 1 for uart 1...), baud rate in bits/second (max/min ranges are board specific)
//        index of the tx pin and rx pin associated with the module (board specific).
//returns: reference to the now setup uart, for using in the other functions
RoveUart_Handle roveUartOpen(uint8_t uart_index, uint32_t baud_rate, uint8_t txPin, uint8_t rxPin);

//writes bytes out on a uart port
//inputs: reference of a setup uart module from roveUartOpen, a pointer to the information to write
//(can be address of a single piece of data, an array, etc), and how many bytes are to be sent.
//returns: Information on how the process went based on roveBoard_ERROR enum
RoveUart_Error roveUartWrite(RoveUart_Handle uart, void* write_buffer, size_t bytes_to_write);

//reads bytes from a uart port
//inputs: reference of a setup uart module from roveUartOpen, a pointer to the buffer to read into
//(can be address of a single piece of data, an array, etc), and how many bytes are to be read.
//returns: Information on how the process went based on roveBoard_ERROR enum
//warning: Blocking, won't return until the uart has that many bytes in its incoming buffer.
RoveUart_Error roveUartRead(RoveUart_Handle uart, void* read_buffer, size_t bytes_to_read);

//clears out the uart receive and transmit buffers
RoveUart_Error roveUartFlushAll(RoveUart_Handle uart);

//reads bytes from a uart port
//inputs: reference of a setup uart module from roveUartOpen, a pointer to the buffer to read into
//(can be address of a single piece of data, an array, etc), and how many bytes are to be read.
//returns: Information on how the process went based on roveBoard_ERROR enum.
//Nonblocking, so if there wasn't that amount of bytes currently in the uart's incoming data buffer it returns error
RoveUart_Error roveUartReadNonBlocking(RoveUart_Handle uart, void* read_buffer, size_t bytes_to_read);

//checks how many bytes the uart currently has in its read buffer
//inputs: reference of a setup uart module from roveUartOpen
//returns: How many bytes the uart currently has in its read buffer
int roveUartAvailable(RoveUart_Handle uart);

//checks what number (a byte) is at the top of the uart's read buffer, without actually taking it out of the buffer
//inputs: reference of a setup uart module from roveUartOpen
//returns: the byte at the top of the uart's read buffer
int roveUartPeek(RoveUart_Handle uart);

//checks what number (a byte) is at some index of the uart's read buffer, without actually taking it out of the buffer
//inputs: reference of a setup uart module from roveUartOpen
//returns: the byte at the index of the uart's read buffer, or -1 if there's nothing at that index
int roveUartPeekIndex(RoveUart_Handle uart, uint16_t peekIndex);

//sets how many bytes the uart module is allowed to use up when saving received messages.
//inputs: reference of a setup uart module from roveUartOpen, and new length of the buffer.
//WARNING: If downsizing the buffer size from its previous size, any data that was currently sitting in the buffer outside of the
//length of the new buffer will be lost
void roveUartSetBufferLength(RoveUart_Handle uart, uint16_t length);

//gets how many bytes the uart module is allowed to use up when saving received messages.
//inputs: reference of a setup uart module from roveUartOpen, and new length of the buffer.
//returns: roveUart receive buffer size
uint16_t roveUartGetBufferLength(RoveUart_Handle uart);

//attaches a function to run whenever a module receives a message, on top of the functions the uart module
//itself runs internally.
//input: a function to run when a receive interrupt is generated. The function itself should have one input that will be
//       filled with arguments by the uart module. The argument will contain the index of the module that interrupted
//note: You can usually attach multiple callbacks if desired. The maximum amount is hardware specific
//warning: It's a good idea not to make these too slow to run; if the uart is constantly receiving a lot of messages, it might
//lead to the callbacks hogging the processor and even having data thrown out due to the receive FIFO being overloaded in the meantime
void roveUartAttachReceiveCb(void (*userFunc)(uint8_t moduleThatReceived));

//attaches a function to run whenever a module is finished transmitting a message or messages, on top of the functions the uart module
//itself runs internally.
//input: a function to run when a transmit interrupt is generated. The function itself should have one input that will be
//       filled with arguments by the uart module. The argument will contain the index of the module that interrupted
//note: You can usually attach multiple callbacks if desired. The maximum amount is hardware specific
void roveUartAttachTransmitCb(void (*userFunc)(uint8_t moduleThatTransmitted));

//modifies how many stop bits are sent and the parity settings of the uart transmissions.
//input: parityBits and stopBits are based on the the constants defined further above, any are acceptable
//returns: any errors encountered
RoveUart_Error roveUartSettings(RoveUart_Handle uart,unsigned int parityBits, unsigned int stopBits);


#endif /* ROVEBOARD_MSP432P401R_API_ROVEUART_ROVEUART_MSP432P401R_H_ */
