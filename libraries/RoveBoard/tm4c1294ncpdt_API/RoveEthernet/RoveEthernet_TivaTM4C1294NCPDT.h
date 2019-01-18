/* Programmer: Gbenga Osibodu
 * Editor/revisor: Drue Satterfield
 * Date of creation: for the original library, who knows, 2015 season I think. Reorganized into roveboard in september 2017.
 * Microcontroller used: Tiva TM4C1294NCPDT
 * Hardware components used by this file: Ethernet hardware
 *    Dependent on System Tick timer, which usually is set up in Clocking.h on the tiva to interrupt every millisecond;
 *    the clocked information that this library uses all depend on that interrupt
 *
 *
 * Description: This library is used to implement UDP networking over the tiva's ethernet port.
 *
 *
 * Warnings: There is currently an unknown error where when too many packets are received at once, a memory leak occurs.
 * See 2016's arm commands for an example of how much proved to be too much. Error did not occur on any other system,
 * as far as could be told, but was proven to be due to internal errors here.
 */

#ifndef ROVEETHERNET_TIVATM4C1294NCPDT_H_
#define ROVEETHERNET_TIVATM4C1294NCPDT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "standardized_API/RoveEthernet.h"

#define ROVE_IP_ADDR_NONE INADDR_NONE
#define UDP_RX_MAX_PACKETS 32
#define UDP_TX_PACKET_MAX_SIZE 2048

//Start up the ethernet hardware and assign an IP to this board. Must be called before anything else
void roveEthernet_NetworkingStart(roveIP myIP);

//Start listening for UDP messages on a given port. Must be called before GetUdpMsg can be called.
RoveEthernet_Error roveEthernet_UdpSocketListen(uint16_t port);

//Stop listening for UDP messages.
RoveEthernet_Error roveEthernet_EndUdpSocket();

//Sends a udp message to the destination IP and port
RoveEthernet_Error roveEthernet_SendUdpPacket(roveIP destIP, uint16_t destPort, const uint8_t* msg, size_t msgSize);

//Checks to see if we've received a udp message since the last time this function was called and returns it if we did.
//Returns-by-pointer the IP of the sending device, and fills up an array that the user must set up themselves
//with the udp message's bytes, up to the size of the users buffer or less.
//Returns Success if there was a udp message in our receive buffer.
RoveEthernet_Error roveEthernet_GetUdpMsg(roveIP* senderIP, void* buffer, size_t bufferSize);

//attach a function to be ran automatically whenever a udp packet is received. Only one allowed. Make sure it isn't too long otherwise
//it could cause a race condition.
//function arguments:   msgBuffer[]: an array of size msgSize that contains all the data bytes that was
//                                   in the udp packet we just received
//                      msgSize:     The size of the data array
//
//function returns:     whether or not to keep this packet in the buffer. If you return true, roveEthernet will keep it in its
//                      internal udp message buffer so that it will appear again when you call GetUdpMsg, after you eventually
//                      process enough packets to bring this one to the top. False means we'll remove the packet from the
//                      buffer when the function returns and you won't see it again
//
//Note:                 Comparing to getUdpMsg: GetUdpMsg gets you the the message at the top of the buffer, this will get you the message
//                      at the bottom of the buffer. GetUdpMsg automatically removes the read packet from the buffer, this will let you choose
//                      to put it back into the buffer or remove it
void roveEthernet_attachUdpReceiveCb(bool (*userFunc)(uint8_t* msgBuffer, size_t msgSize));

#endif
