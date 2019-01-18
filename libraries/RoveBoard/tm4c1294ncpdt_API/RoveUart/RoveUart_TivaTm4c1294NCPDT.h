/* Programmer: Gbenga Osibodu
 * Editor/revisor: Drue Satterfield
 * Date of creation: for the original library, who knows, 2015 season I think. Reorganized into roveboard in september 2017.
 * Microcontroller used: Tiva TM4C1294NCPDT
 * Hardware components used by this file: all Uart modules, 0 to 7
 *
 *
 * Description: This library is used to implement basic Serial communication via the hardware UART modules.
 * Standard functions are implemented for receiving and transmitting series's of bytes, and also nonstandard for
 * changing the parity settings, amount of stop bits per transmission, and transmission/receive packet length.
 * Uart0 is wired to talk to the debugger on a computer instead of external devices unless the hardware designer connected them to different things
 * Those pins are:
 *
 * Module      TX           RX
 *   0        PA_1         PA_0
 *   1        PB_1         PB_0
 *   2    PA_7, PD_5    PA_6, PD_4
 *   3    PA_5, PJ_1    PA_4, PJ_0
 *   4    PK_1, PA_3    PK_0, PA_2
 *   5        PC_7         PC_6
 *   6        PP_1         PP_0
 *   7        PC_5         PC_4
 *
 * Warnings:
 * This library uses 512 bytes of dynamic memory per uart initialized
 *
 */

#ifndef ROVEBOARD_TM4C1294NCPDT_ROVEBOARD_TM4C1294NCPDT_ROVEUART_ROVEUART_TIVATM4C1294NCPDT_H_
#define ROVEBOARD_TM4C1294NCPDT_ROVEBOARD_TM4C1294NCPDT_ROVEUART_ROVEUART_TIVATM4C1294NCPDT_H_

#include <stddef.h>

#include "../../standardized_API/RoveUart.h"

//roveUartSettings arguments:
//constants to set word length in transmission in bits
#define WordLength8 0
#define WordLength7 1
#define WordLength6 2
#define WordLength5 3

//constants to set stop bits in transmission
#define OneStopBit 0
#define TwoStopBit 1

//constants to set up parity bits in transmission
#define NoParity 0
#define EvenParity 1
#define OddParity 2
#define AlwaysZero 3
#define AlwaysOne 4

//sets up the specified uart to run at the specified baud rate
//inputs: index of the uart module to run (0 for uart 0, 1 for uart 1...up to 7), baud rate in bits/second. Max is 7.5 Mb/s
//        rxPin and to use with the uart Module, refer to the header global comment to see what works with what.
//returns: reference to the now setup uart, for using in the other functions
//warning: Though it can go up to 7.5 Mb/s, the hardware grows more difficult to work with the higher the speed. Unless
//the electricals set up the board specifically for high speed communication, it's usually best to stay below 1Mb/s if talking to another
//microcontroller; if talking to preset IC, then use whatever baud rate it naturally supports.
//warning: function enters infinite fault loop if arguments are wrong (IE if uart_index isn't from 0 to 7 or wrong pins entered)

//writes bytes out on a uart port
//inputs: reference of a setup uart module from roveUartOpen, a pointer to the information to write
//(can be address of a single piece of data, an array, etc), and how many bytes are to be sent.
//returns: Information on how the process went based on RoveUart_Error enum
//warning: Function is blocking, will stay in function until the information is written out.
RoveUart_Handle roveUartOpen(uint8_t uart_index, uint32_t baud_rate, uint8_t txPin, uint8_t rxPin);

//reads bytes from a uart port
//inputs: reference of a setup uart module from roveUartOpen, a pointer to the buffer to read into
//(can be address of a single piece of data, an array, etc), and how many bytes are to be read.
//returns: Information on how the process went based on RoveUart_Error enum
//warning: Blocking, won't return until the uart has that many bytes in its incoming buffer.
RoveUart_Error roveUartRead(RoveUart_Handle uart, void* read_buffer, size_t bytes_to_read);

//reads bytes from a uart port
//inputs: reference of a setup uart module from roveUartOpen, a pointer to the buffer to read into
//(can be address of a single piece of data, an array, etc), and how many bytes are to be read.
//returns: Information on how the process went based on RoveUart_Error enum.
//Nonblocking, so if there wasn't that amount of bytes currently in the uart's incoming data buffer it returns error
RoveUart_Error roveUartReadNonBlocking(RoveUart_Handle uart, void* read_buffer, size_t bytes_to_read);

//checks how many bytes the uart currently has in its read buffer
//inputs: reference of a setup uart module from roveUartOpen
//returns: How many bytes the uart currently has in its read buffer
int roveUartAvailable(RoveUart_Handle uart);

//clears out the uart receive and transmit buffers
RoveUart_Error roveUartFlushAll(RoveUart_Handle uart);

//checks what number (a byte) is at the top of the uart's read buffer, without actually taking it out of the buffer
//inputs: reference of a setup uart module from roveUartOpen
//returns: the byte at the top of the uart's read buffer, or -1 if there's nothing in the buffer
int roveUartPeek(RoveUart_Handle uart);

//checks what number (a byte) is at some index of the uart's read buffer, without actually taking it out of the buffer
//inputs: reference of a setup uart module from roveUartOpen
//returns: the byte at the index of the uart's read buffer, or -1 if there's nothing at that index
int roveUartPeekIndex(RoveUart_Handle uart, uint16_t peekIndex);

//changes the uart's output settings such as settings about its parity bit, stop bits, and word length
//inputs: reference of a setup uart module from roveUartOpen, and arguments based on the constants defined above
//returns: Information on how the process went based on RoveUart_Error enum.
RoveUart_Error roveUartSettings(RoveUart_Handle uart, unsigned int parityBits, unsigned int stopBits, unsigned int wordLength);

//sets how many bytes the uart module is allowed to use up when saving received messages. The uart module interrupts when it receives
//a message and stores it into a buffer for the user to access when roveUartRead is called; this determines how many bytes the module
//will be allowed to save without the user calling roveUartRead before it has to start ignoring new messages until read is called which
//takes the read bytes out of the buffer
//inputs: reference of a setup uart module from roveUartOpen, and new length of the buffer.
//WARNING: If downsizing the buffer size from its previous size, any data that was currently sitting in the buffer outside of the
//length of the new buffer will be lost
void roveUartSetBufferLength(RoveUart_Handle uart, uint16_t length);

//gets how many bytes the uart module is allowed to use up when saving received messages.
//inputs: reference of a setup uart module from roveUartOpen, and new length of the buffer.
//returns: roveUart receive buffer size
//note: default buffer size is 256
uint16_t roveUartGetBufferLength(RoveUart_Handle uart);

//attaches a function to run whenever a module receives a message, on top of the functions the uart module
//itself runs internally.
//input: a function to run when a receive interrupt is generated. The function itself should have one input that will be
//       filled with arguments by the uart module. The argument will contain the index of the module that interrupted
//note: You can attach multiple callbacks if desired. The maximum amount is 8
//warning: It's a good idea not to make these too slow to run; if the uart is constantly receiving a lot of messages, it might
//lead to the callbacks hogging the processor and even having data thrown out due to the receive FIFO being overloaded in the meantime
void roveUartAttachReceiveCb(void (*userFunc)(uint8_t moduleThatReceived));

//attaches a function to run whenever a module is finished transmitting a message or messages, on top of the functions the uart module
//itself runs internally.
//input: a function to run when a transmit interrupt is generated. The function itself should have one input that will be
//       filled with arguments by the uart module. The argument will contain the index of the module that interrupted
//note: You can attach multiple callbacks if desired. The maximum amount is 8
void roveUartAttachTransmitCb(void (*userFunc)(uint8_t moduleThatTransmitted));



#endif /* ROVEBOARD_TM4C1294NCPDT_ROVEBOARD_TM4C1294NCPDT_ROVEUART_ROVEUART_TIVATM4C1294NCPDT_H_ */
