/**
 * @file can.h
 *
 * A driver for the <a href="http://www.microchip.com/wwwproducts/en/MCP2515">MCP2515</a> CAN controller.
 *
 * This driver was initially implemented by <a href="http://tritium.com.au">Tritium</a> in their EV driver controls project,
 * and then ported by our team to our platforms.
 *
 * @date 28 October 2014
 *
 * @author Jesse Cureton
 * @author Caleb Olson
 * @author William Lorey
 */

#ifndef CAN_H_
#define CAN_H_

#include "datatypes.h"

//Board Specific Information:

//CAN Message flags, specific to the BMS
//These are based on the {@link CAN_SPEC} message descriptions
//#define BMS_BASE			CAN_BASE
#define BMS_STATUS			BMS_BASE
#define BMS_VTG1			BMS_BASE + 1
#define BMS_VTG2			BMS_BASE + 2
#define BMS_TEMP1			BMS_BASE + 3
#define BMS_TEMP2			BMS_BASE + 4
#define BMS_TEMP3			BMS_BASE + 5
#define BMS_TEMP4			BMS_BASE + 6
#define BMS_CURRENT			BMS_BASE + 7
#define BMS_RESISTANCE		BMS_BASE + 8
#define BMS_RESISTANCE2		BMS_BASE + 9

#define GLOBAL_ESTOP		BMS_BASE + 0x10

#define DE_STATUS			DE_BASE
#define DE_BASE				0x500			//Driver electronics base address

#define MC_BASE				0x400			//Motor controller base address
#define MC_BUS_MEASURE		MC_BASE + 2		//Motor controller bus voltage/current packet

//Receive addresses
#define BOARD_RX1_MASK		0x07FF			///< Board receive mask bits. BMS wants to check all 11 bits @see {@link RX_MASK_0}
#define BOARD_RX1_FILTER1	DE_STATUS		///< Receive filter address 1. We receive the DE status message. @see {@link RX_ID_0A}
#define BOARD_RX1_FILTER2	GLOBAL_ESTOP	///< Receive filter address 2. We receive the global e-stop packet. @see {@link RX_ID_0B}

#define BOARD_RX2_MASK		0x07FF
#define BOARD_RX2_FILTER1	MC_BUS_MEASURE
#define BOARD_RX2_FILTER2	0x0000
#define BOARD_RX2_FILTER3	0x0000
#define BOARD_RX2_FILTER4	0x0000

//BMS doesn't want to enable receive buffer rollover, since we use both receive buffers
#define BOARD_ENABLE_RX_ROLLOVER	FALSE

//Macros
#define can_select		P8OUT &= ~CAN_CS	///< Select the MCP2515 over SPI by pulling {@link CAN_CS} low.
#define can_deselect	P8OUT |= CAN_CS 	///< Release the MCP2515 over SPI by releasing {@link CAN_CS} high.

//Generic driver code
#ifndef can_select
#error "You must define a can_select macro that will pull the chip-select for the chip low."
#endif

#ifndef can_deselect
#error "You must define a can_deselect macro that will release the chip-select for the chip high."
#endif

//Public functions
//	I know, I know, C has no concept of public vs private functions. These are the ones you should call externally. Others live only in this driver.
void can_init(void);
void can_receive(void);
void can_sleep(void);
void can_wake(void);
void can_push(uint8_t buffer);
void can_read(uint8_t address, uint8_t *ptr, uint8_t bytes);
can_message* can_pop(uint8_t buffer);
int8_t can_transmit(void);
int8_t can_isBusy(void);
void can_mod(uint8_t address, uint8_t mask, uint8_t data);

/**
 * @defgroup CAN_BUFFERS CAN Buffers
 *
 * @{
 */
#define CAN_RX	0	///< CAN Receive buffer
#define CAN_TX	1	///< CAN Transmit buffer
/** @} */

#define CAN_BUF_LEN 	32					///< The length of CAN TX/RX buffers, in CAN # messages.
/**
 * The global-scope CAN transmit queue.
 *
 * Should never be accessed directly. Instead use the {@link can_push()} and {@link can_pop()} functions.
 * To push a message into the queue, use the {@link can_tx_push_ptr} pointer to set your data, then call
 * {@link can_push()}.
 *
 * To read a message out of the queue, call {@link can_pop()} and then read the members of the returned pointer.
 */
extern can_message canq_tx[CAN_BUF_LEN];
extern can_message *can_tx_push_ptr;	///< Pointer to the location to push a message into the {@link canq_tx CAN transmit queue}
extern can_message *can_tx_pop_ptr;		///< Pointer to the location to pop a message out of the {@link canq_tx CAN transmit queue}
/**
 * The global-scope CAN receive queue.
 *
 * Should never be accessed directly. Instead use the {@link can_push()} and {@link can_pop()} functions.
 * To push a message into the queue, use the {@link can_rx_push_ptr} pointer to set your data, then call
 * {@link can_push()}.
 *
 * To read a message out of the queue, call {@link can_pop()} and then read the members of the returned pointer.
 */
extern can_message canq_rx[CAN_BUF_LEN];
extern can_message *can_rx_push_ptr;///< Pointer to the location to push a message into the {@link canq_rx CAN receive queue}
extern can_message *can_rx_pop_ptr;	///< Pointer to the location to pop a message out of the {@link canq_rx CAN receive queue}

//RX Buffer 0
/**
 * Receive mask for RX buffer 0 (RXB0).
 *
 * A mask acts as an initial filter to mark what bits in the incoming message ID that the filters will examine.
 * I.E if we only care about the upper 6 bits of the 11 bit address, a mask of 0x07E0 will allow anything matching
 * the upper 6 bits of the filter addresses {@link RX_ID_0A} and {@link RX_ID_0B} into the receive buffer, and all
 * others will be rejected. Similarly, all 0s in the mask field will put ALL received messages in the RXB0 buffer,
 * and all 1s in the mask will result in every bit being checked against the filter addresses.
 *
 * We define this as BOARD_RX_MASK, which must be defined somewhere in platform configuration. If it isn't defined, we throw a compiler error.
 *
 * @see <a href="ww1.microchip.com/downloads/en/DeviceDoc/21801G.pdf>Datasheet</a> section 4.5
 */
#define RX_MASK_0 	BOARD_RX1_MASK
#ifndef BOARD_RX1_MASK
#error "You must define all board receive masks in your board configuration! If you aren't receiving CAN messages, use 0x07FF. See RX_MASK_0 documentation for more details."
#endif
/**
 * Receive filter A address for RXB0.
 *
 * For the bits described in {@link RX_MASK_0}, an incoming message address will be checked against this filter.
 * If it matches, the message will be placed in RXB0. If it doesn't, it will move down to check against the next filter.
 * If no filters match, it will be declined and not placed in a receive buffer.
 *
 * @see <a href="ww1.microchip.com/downloads/en/DeviceDoc/21801G.pdf>Datasheet</a> section 4.5
 */
#define RX_ID_0A	BOARD_RX1_FILTER1
#ifndef BOARD_RX1_FILTER1
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_ID_0A documentation for more details"
#endif
/**
 * Receive filter B address for RXB0.
 *
 * For the bits described in {@link RX_MASK_0}, an incoming message address will be checked against this filter.
 * If it matches, the message will be placed in RXB0. If it doesn't, it will move down to check against the next filter.
 * If no filters match, it will be declined and not placed in a receive buffer.
 *
 * @see <a href="ww1.microchip.com/downloads/en/DeviceDoc/21801G.pdf>Datasheet</a> section 4.5
 */
#define RX_ID_0B	BOARD_RX1_FILTER2
#ifndef BOARD_RX1_FILTER2
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_ID_0B documentation for more details"
#endif

//RX Buffer 1 - MCP2515 has 2
#define RX_MASK_1	BOARD_RX2_MASK		///< Receive mask for receive buffer 1 (RXB1). {@see RX_MASK_0}
#ifndef BOARD_RX2_MASK
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_MASK_1 documentation for more details"
#endif
#define RX_ID_1A	BOARD_RX2_FILTER1	///< Receive filter 1 address for receive buffer 1. {@see RX_ID_0A}
#ifndef BOARD_RX2_FILTER1
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_ID_1A documentation for more details"
#endif
#define RX_ID_1B	BOARD_RX2_FILTER2	///< Receive filter 2 address for receive buffer 1. {@see RX_ID_0A}
#ifndef BOARD_RX2_FILTER1
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_ID_1B documentation for more details"
#endif
#define RX_ID_1C	BOARD_RX2_FILTER3	///< Receive filter 3 address for receive buffer 1. {@see RX_ID_0A}
#ifndef BOARD_RX2_FILTER1
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_ID_1C documentation for more details"
#endif
#define RX_ID_1D	BOARD_RX2_FILTER4	///< Receive filter 4 address for receive buffer 1. {@see RX_ID_0A}
#ifndef BOARD_RX2_FILTER1
#error "You must define all receive filters in your board configuration! If you aren't receiving CAN messages, use 0x0000. See RX_ID_1D documentation for more details"
#endif

#define RXB_ROLLOVER_ENABLED	BOARD_ENABLE_RX_ROLLOVER	///< Whether or not to enable the BUKT bit to allow rollover from RXB0 to RXB1 if RXB0 is full. Value of 0 disables rollover, anything else enables it. {@see BOARD_ENABLE_RX_ROLLOVER}
#ifndef BOARD_ENABLE_RX_ROLLOVER
#error "You must define whether to allow receive buffer overflow or not. See BOARD_ENABLE_RX_ROLLOVER."
#endif

/**
 * @defgroup CAN_MSG_STATUS CAN Message Statuses
 *
 * These define the status bytes of a received CAN message. Only useful on CAN reception, and need not be set on a message for transmission.
 *
 * @{
 */
#define CAN_ERROR		0xFFFF	///< CAN Error
#define CAN_MERROR		0xFFFE
#define CAN_WAKE		0xFFFD
#define CAN_RTR			0xFFFC	///< CAN Return request packet
#define CAN_OK			0x0001	///< Good CAN message
/** @} */

volatile uint8_t CAN_BUSY;

// MCP2515 command bytes
#define MCP_RESET		0xC0
#define MCP_READ		0x03
#define MCP_READ_RX		0x90
#define MCP_WRITE		0x02
#define MCP_WRITE_TX	0x40
#define MCP_RTS			0x80		// When used, needs to have buffer to transmit inserted into lower bits
#define MCP_STATUS		0xA0
#define MCP_FILTER		0xB0
#define MCP_MODIFY		0x05

// MCP2515 register names
#define RXF0SIDH		0x00
#define RXF0SIDL		0x01
#define RXF0EID8		0x02
#define RXF0EID0		0x03
#define RXF1SIDH		0x04
#define RXF1SIDL		0x05
#define RXF1EID8		0x06
#define RXF1EID0		0x07
#define RXF2SIDH		0x08
#define RXF2SIDL		0x09
#define RXF2EID8		0x0A
#define RXF2EID0		0x0B
#define BFPCTRL			0x0C
#define TXRTSCTRL		0x0D
#define CANSTAT			0x0E
#define CANCTRL			0x0F

#define RXF3SIDH		0x10
#define RXF3SIDL		0x11
#define RXF3EID8		0x12
#define RXF3EID0		0x13
#define RXF4SIDH		0x14
#define RXF4SIDL		0x15
#define RXF4EID8		0x16
#define RXF4EID0		0x17
#define RXF5SIDH		0x18
#define RXF5SIDL		0x19
#define RXF5EID8		0x1A
#define RXF5EID0		0x1B
#define TEC				0x1C
#define REC				0x1D

#define RXM0SIDH		0x20
#define RXM0SIDL		0x21
#define RXM0EID8		0x22
#define RXM0EID0		0x23
#define RXM1SIDH		0x24
#define RXM1SIDL		0x25
#define RXM1EID8		0x26
#define RXM1EID0		0x27
#define CNF3			0x28
#define CNF2			0x29
#define CNF1			0x2A
#define CANINTE			0x2B
#define CANINTF			0x2C
#define EFLAG			0x2D

#define TXB0CTRL		0x30
#define TXB0SIDH		0x31
#define TXB0SIDL		0x32
#define TXB0EID8		0x33
#define TXB0EID0		0x34
#define TXB0DLC			0x35
#define TXB0D0			0x36
#define TXB0D1			0x37
#define TXB0D2			0x38
#define TXB0D3			0x39
#define TXB0D4			0x3A
#define TXB0D5			0x3B
#define TXB0D6			0x3C
#define TXB0D7			0x3D

#define TXB1CTRL		0x40
#define TXB1SIDH		0x41
#define TXB1SIDL		0x42
#define TXB1EID8		0x43
#define TXB1EID0		0x44
#define TXB1DLC			0x45
#define TXB1D0			0x46
#define TXB1D1			0x47
#define TXB1D2			0x48
#define TXB1D3			0x49
#define TXB1D4			0x4A
#define TXB1D5			0x4B
#define TXB1D6			0x4C
#define TXB1D7			0x4D

#define TXB2CTRL		0x50
#define TXB2SIDH		0x51
#define TXB2SIDL		0x52
#define TXB2EID8		0x53
#define TXB2EID0		0x54
#define TXB2DLC			0x55
#define TXB2D0			0x56
#define TXB2D1			0x57
#define TXB2D2			0x58
#define TXB2D3			0x59
#define TXB2D4			0x5A
#define TXB2D5			0x5B
#define TXB2D6			0x5C
#define TXB2D7			0x5D

#define RXB0CTRL		0x60
#define RXB0SIDH		0x61
#define RXB0SIDL		0x62
#define RXB0EID8		0x63
#define RXB0EID0		0x64
#define RXB0DLC			0x65
#define RXB0D0			0x66
#define RXB0D1			0x67
#define RXB0D2			0x68
#define RXB0D3			0x69
#define RXB0D4			0x6A
#define RXB0D5			0x6B
#define RXB0D6			0x6C
#define RXB0D7			0x6D

#define RXB1CTRL		0x70
#define RXB1SIDH		0x71
#define RXB1SIDL		0x72
#define RXB1EID8		0x73
#define RXB1EID0		0x74
#define RXB1DLC			0x75
#define RXB1D0			0x76
#define RXB1D1			0x77
#define RXB1D2			0x78
#define RXB1D3			0x79
#define RXB1D4			0x7A
#define RXB1D5			0x7B
#define RXB1D6			0x7C
#define RXB1D7			0x7D

// MCP2515 RX ctrl bit definitions
#define MCP_RXB0_RTR	0x08
#define MCP_RXB0_BUKT	0x04
#define MCP_RXB1_RTR	0x08

// MCP2515 Interrupt flag register bit definitions
#define MCP_IRQ_MERR	0x80
#define MCP_IRQ_WAKE	0x40
#define MCP_IRQ_ERR		0x20
#define MCP_IRQ_TXB2	0x10
#define MCP_IRQ_TXB1	0x08
#define MCP_IRQ_TXB0	0x04
#define MCP_IRQ_RXB1	0x02
#define MCP_IRQ_RXB0	0x01

#endif /* CAN_H_ */
