// roveBoard.h for Tiva/Energia
// Author: Gbenga Osibodu
// Second Author: Drue Satterfield
// Heavily based off of the Energia framwork's Ethernetclass and EthernetUdpclass

#include "RoveEthernet_TivaTM4C1294NCPDT.h"
#include <stdbool.h>
#include <String.h>
#include <stdint.h>
#include <stddef.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/arch/lwiplib.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/lwip/dns.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/lwip/err.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/lwip/inet.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/lwip/lwipopts.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/lwip/udp.h>
#include <tm4c1294ncpdt_API/RoveEthernet/lwipLibrary/netif/tivaif.h>
#include "supportingUtilities/IPAddress.h"
#include "../tivaware/inc/hw_ints.h"
#include "../Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "../tivaware/inc/hw_flash.h"
#include "../tivaware/inc/hw_memmap.h"
#include "../RovePinMap_TivaTM4C1294NCPDT.h"
#include "../tivaware/driverlib/rom.h"
#include "../tivaware/driverlib/flash.h"
#include "../tivaware/driverlib/interrupt.h"
#include "../tivaware/driverlib/gpio.h"
#include "../tivaware/driverlib/pin_map.h"
#include "../tivaware/driverlib/emac.h"

#define CLASS_A 0x0
#define CLASS_B 0x2
#define ETHERNET_INT_PRIORITY   0xC0
#define CLASS_C 0x6

static uint8_t macArray[8];
static const uint8_t MaxCallbacks = 1;
static bool (*receiveCbFuncs[MaxCallbacks])(uint8_t* msgBuffer, size_t msgSize);
static const IPAddress CLASS_A_SUBNET(255, 0, 0, 0);
static const IPAddress CLASS_B_SUBNET(255, 255, 0, 0);
static const IPAddress CLASS_C_SUBNET(255, 255, 255, 0);

struct packet {
  struct pbuf *p;
  IPAddress remoteIP;
  uint16_t remotePort;
  IPAddress destIP;
};

typedef struct
{
  struct packet packets[UDP_RX_MAX_PACKETS];
  uint8_t front;
  uint8_t rear;
  uint8_t count;

  struct udp_pcb *_pcb;
  struct pbuf *_p;
  uint16_t _port;
  /* IP and port filled in when receiving a packet */
  IPAddress _remoteIP;
  uint16_t _remotePort;
  IPAddress _destIP;
  /* pbuf, pcb, IP and port used when acting as a client */
  struct pbuf *_sendTop;
  struct udp_pcb *_sendToPcb;
  IPAddress _sendToIP;
  uint16_t _sendToPort;

  uint16_t _read;
  uint16_t _write;
}UdpData;

static UdpData data;
static bool weHazCallbacks = false;
void do_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr* addr, uint16_t port);
static bool beginUdpPacket(IPAddress ip, uint16_t port);
static size_t writeUdpPacket(const uint8_t *buffer, size_t size);
static int readUdp(unsigned char* buffer, size_t len);
static int readUdp();
static int parseUdpPacket();
static bool endUdpPacket();
static int udpAvailable();
static void stopUdp();
static void startEthernetHardware(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
static void readUdpPacket(pbuf *p, uint16_t lengthOfPacket, uint8_t* returnByteBuffer);

extern void lwIPEthernetIntHandler(void);


void roveEthernet_NetworkingStart(roveIP myIP)
{
  /* Assume the DNS server will be the machine on the same network as the local IP
   * but with last octet being '1' */
  IPAddress dns_server = myIP;
  dns_server[3] = 1;

  /* Assume the gateway will be the machine on the same network as the local IP
   * but with last octet being '1' */
  IPAddress gateway = myIP;
  gateway[3] = 1;

  startEthernetHardware(myIP, dns_server, gateway, IPAddress(0,0,0,0));

  //enable link and activity led's so the users can visually see when ethernet exchange is happening
  GPIOPinConfigure(LINK_LED);
  GPIOPinTypeEthernetLED(LINK_LED_BASE, LINK_LED_PIN);
  GPIOPinConfigure(ACTIVITY_LED);
  GPIOPinTypeEthernetLED(ACTIVITY_LED_BASE, ACTIVITY_LED_PIN);
}

RoveEthernet_Error roveEthernet_UdpSocketListen(uint16_t port)
{
  data._port = port;

  if(data._pcb != 0)
  {
    stopUdp(); //catch attempts to re-init
  }

  data._pcb = udp_new();
  err_t err = udp_bind(data._pcb, IP_ADDR_ANY, port);

  if(err == ERR_USE)
    return ROVE_ETHERNET_ERROR_UNKNOWN;


  udp_recv(data._pcb, do_recv, &data);

  return ROVE_ETHERNET_ERROR_SUCCESS;
}

RoveEthernet_Error roveEthernet_EndUdpSocket()
{
  stopUdp();

  return ROVE_ETHERNET_ERROR_SUCCESS;
}

RoveEthernet_Error roveEthernet_SendUdpPacket(roveIP destIP, uint16_t destPort, const uint8_t* msg, size_t msgSize)
{
  beginUdpPacket(destIP, destPort);
  writeUdpPacket(msg, msgSize);
  endUdpPacket();
  return ROVE_ETHERNET_ERROR_SUCCESS;
}

RoveEthernet_Error roveEthernet_GetUdpMsg(roveIP* senderIP, void* buffer, size_t bufferSize)
{
  int packetSize = parseUdpPacket();
  
  if (packetSize > 0) //if there is a packet available
  {
    readUdp((unsigned char*)buffer, bufferSize);
    *senderIP = data._remoteIP;
    return ROVE_ETHERNET_ERROR_SUCCESS;
  }
  else
  {
    return ROVE_ETHERNET_ERROR_WOULD_BLOCK;
  }
}

void roveEthernet_attachUdpReceiveCb(bool (*userFunc)(uint8_t* msgBuffer, size_t msgSize))
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

  weHazCallbacks = true;
}

static void startEthernetHardware(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
  unsigned long F_CPU = getCpuClockFreq();
  uint32_t ui32User0, ui32User1;
  EMACIntRegister(EMAC0_BASE, lwIPEthernetIntHandler);
  registerSysTickCb(lwIPTimer);
  FlashUserGet(&ui32User0, &ui32User1);

  /*
   * The tiva keeps its MAC address built into its NV ram.
   * Convert the 24/24 split MAC address from NV ram into a 32/16 split
   * MAC address needed to program the hardware registers, then program
   * the MAC address into the Ethernet Controller registers.
   */
  macArray[0] = ((ui32User0 >>  0) & 0xff);
  macArray[1] = ((ui32User0 >>  8) & 0xff);
  macArray[2] = ((ui32User0 >> 16) & 0xff);
  macArray[3] = ((ui32User1 >>  0) & 0xff);
  macArray[4] = ((ui32User1 >>  8) & 0xff);
  macArray[5] = ((ui32User1 >> 16) & 0xff);

  IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);

  if(!subnet) {
    if((local_ip >> 31) == CLASS_A)
      subnet = CLASS_A_SUBNET;
    else if((local_ip >> 30) == CLASS_B)
      subnet = CLASS_B_SUBNET;
    else if((local_ip >> 29) == CLASS_C)
      subnet = CLASS_C_SUBNET;
  }
  //delayMicroseconds(100); //causes hard faults on linux for some reason.
  lwIPInit(F_CPU, macArray, htonl(local_ip), htonl(subnet), htonl(gateway), !local_ip ? IPADDR_USE_DHCP:IPADDR_USE_STATIC);
  delayMicroseconds(100);
  lwIPDNSAddrSet((uint32_t)dns_server);
}

static bool beginUdpPacket(IPAddress ip, uint16_t port)
{
  data._sendToIP = ip;
  data._sendToPort = port;

  data._sendTop = pbuf_alloc(PBUF_TRANSPORT, UDP_TX_PACKET_MAX_SIZE, PBUF_POOL);

  data._write = 0;

  if(data._sendTop == NULL)
    return false;

  return true;
}

static size_t writeUdpPacket(const uint8_t *buffer, size_t size)
{
  uint16_t avail = data._sendTop->tot_len - data._write;

  /* If there is no more space available
   * then return immediately */
  if(avail == 0)
    return 0;

  /* If size to send is larger than is available,
   * then only send up to the space available */
  if(size > avail)
    size = avail;

  /* Copy buffer into the pbuf */
  pbuf_take(data._sendTop, buffer, size);

  data._write += size;

  return size;
}

static bool endUdpPacket()
{
  ip_addr_t dest;
  dest.addr = data._sendToIP;

  /* Shrink the pbuf to the actual size that was written to it */
  pbuf_realloc(data._sendTop, data._write);

  /* Send the buffer to the remote host */
  err_t err = udp_sendto(data._pcb, data._sendTop, &dest, data._sendToPort);

  /* udp_sendto is blocking and the pbuf is
   * no longer needed so free it */
  pbuf_free(data._sendTop);

  if(err != ERR_OK)
    return false;

  return true;
}

static int parseUdpPacket()
{
  data._read = 0;

  /* Discard the current packet */
  if(data._p) {
    pbuf_free(data._p);
    data._p = 0;
    data._remotePort = 0;
    data._remoteIP = IPAddress(IPADDR_NONE);
    data._destIP = IPAddress(IPADDR_NONE);
  }

  /* No more packets in the queue */
  if(!data.count) {
    return 0;
  }

  /* Take the next packet from the front of the queue */
  data._p = data.packets[data.front].p;
  data._remoteIP = data.packets[data.front].remoteIP;
  data._remotePort = data.packets[data.front].remotePort;
  data._destIP = data.packets[data.front].destIP;

  data.count--;

  /* Advance the front of the queue */
  data.front++;

  /* Wrap around if end of queue has been reached */
  if(data.front == UDP_RX_MAX_PACKETS)
    data.front = 0;

  /* Return the total len of the queue */
  return data._p->tot_len;
}

void do_recv(void *args, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr* addr, uint16_t port)
{
  UdpData *udp = static_cast<UdpData*>(args);

  /* No more space in the receive queue */
  if(udp->count >= UDP_RX_MAX_PACKETS) {
    pbuf_free(p);
  }
  else
  {
    bool keepPacket = true;

    //run any user attached callbacks for receiving a udp packet
    if(weHazCallbacks)
    {
      uint8_t i;
      uint16_t packetLength = p->len;
      uint8_t buff[packetLength];
      readUdpPacket(p, packetLength, buff);

      for(i = 0; i < MaxCallbacks; i++)
      {
        if(receiveCbFuncs[i])
        {
          keepPacket = receiveCbFuncs[i](buff, packetLength);
        }
      }
    }


    if(keepPacket == false)
    {
      pbuf_free(p);
    }
    else
    {
      /* Increase the number of packets in the queue
       * that are waiting for processing */
      udp->count++;
      /* Add pacekt to the rear of the queue */
      udp->packets[udp->rear].p = p;
      /* Record the IP address and port the pacekt was received from */
      udp->packets[udp->rear].remoteIP = IPAddress(addr->addr);
      udp->packets[udp->rear].remotePort = port;
      udp->packets[udp->rear].destIP = IPAddress(ip_current_dest_addr()->addr);

      /* Advance the rear of the queue */
      udp->rear++;

      /* Wrap around the end of the array was reached */
      if(udp->rear == UDP_RX_MAX_PACKETS)
        udp->rear = 0;
    }
  }
}

static int readUdp(unsigned char* buffer, size_t len)
{
  uint16_t avail = udpAvailable();
  uint16_t i;
  int b;

  if(!avail)
    return -1;

  for(i = 0; i < len; i++) {
    b = readUdp();
    if(b == -1)
      break;
    buffer[i] = b;
  }

  return i;
}

static void readUdpPacket(pbuf *p, uint16_t lengthOfPacket, uint8_t* returnByteBuffer)
{
  int i;
  uint8_t *buf = (uint8_t *)data._p->payload;

  for(i = 0; i < lengthOfPacket; i++)
  {
    if(i >=  p->len)
    {
      break;
    }

    returnByteBuffer[i] = buf[i];
  }
}

static int readUdp()
{
  if(!udpAvailable()) return -1;

  uint8_t *buf = (uint8_t *)data._p->payload;
  uint8_t b = buf[data._read];
  data._read = data._read + 1;

  if((data._read == data._p->len) && data._p->next) {
    data._read = 0;
    pbuf *p;
    p = data._p->next;
    /* Increase ref count on p->next
     * 1->2->1->etc */
    pbuf_ref(data._p->next);
    /* Free p which decreases ref count of the chain
     * and frees up to p->next in this case
     * ...->1->1->etc */
    pbuf_free(data._p);
    data._p = 0;
    data. _p = p;
  } else if(data._read == data._p->len) {
    data._read = 0;
    pbuf_free(data._p);
    data._p = 0;
  }

  return b;
}

static int udpAvailable()
{
  if(!data._p)
    return 0;

  return data._p->tot_len - data._read;
}

static void stopUdp()
{
  udp_remove(data._pcb);
  data._pcb = 0;
}
