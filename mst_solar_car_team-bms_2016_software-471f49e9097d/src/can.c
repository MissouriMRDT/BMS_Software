/**
 * @file can.c
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

#include <msp430.h>
#include "main.h"
#include "pins.h"
#include "can.h"
#include "spi.h"
#include "datatypes.h"
#include "config.h"

can_message canq_tx[CAN_BUF_LEN];
can_message *can_tx_push_ptr;
can_message *can_tx_pop_ptr;

can_message canq_rx[CAN_BUF_LEN];
can_message *can_rx_push_ptr;
can_message *can_rx_pop_ptr;

uint8_t buffer[16];

//'Private function prototypes
static void		can_reset(void);
static void		can_write(uint8_t address, uint8_t *ptr, uint8_t bytes);
static void		can_write_tx(uint8_t *ptr);
static void		can_rts(uint8_t address);
static uint8_t	can_read_status(void);
static uint8_t	can_read_filter(void);

// Public function implementations

/**
 * Initialize the MCP2515.
 */
void can_init(void)  //Here there be dragons
{
	uint16_t i;
	//set up transmit and receive buffers
	can_tx_push_ptr = canq_tx;
	can_tx_pop_ptr = can_tx_push_ptr;

	can_rx_push_ptr = canq_rx;
	can_rx_pop_ptr = can_rx_push_ptr;

	//set up reset & clocking
	can_reset();
	for(i = 0; i < 1000; i++); //wait for MCP2515 to finish reset
	can_mod(CANCTRL, 0x03, 0x00);  //CANCTRL register, modify lower two bits, clk = /1 = 20MHz

	//Set up CAN bitrate for 500kbps
	buffer[0] = 0x05;
	buffer[1] = 0xF8;
	buffer[2] = 0x00;

	//Set up interrupts
	buffer[3] = 0x23; 	//CANINTE register: enable error, rx0 & rx1 interrupts on IRQ pin
	buffer[4] = 0x00;	//CANINTF register: clear all IRQ flags
	buffer[5] = 0x00;   //EFLG register: clear all user-changable error flags
	can_write(CNF3, &buffer[0], 6);

	//Enable receive buffer rollover if it is enabled in the driver
	if(RXB_ROLLOVER_ENABLED == FALSE)
	{
		can_mod(RXB0CTRL, MCP_RXB0_BUKT, 0x00);	//BUKT bit low, rollover disabled
	} else {
		can_mod(RXB0CTRL, MCP_RXB0_BUKT, 0x04);	//BUKT bit high, rollover enabled
	}

	// Set up receive filtering & masks
	// RXF0 - Buffer 0
	buffer[ 0] = (uint8_t)(RX_ID_0A >> 3);
	buffer[ 1] = (uint8_t)(RX_ID_0A << 5);
	buffer[ 2] = 0x00;
	buffer[ 3] = 0x00;
	// RXF1 - Buffer 0
	buffer[ 4] = (uint8_t)(RX_ID_0B >> 3);
	buffer[ 5] = (uint8_t)(RX_ID_0B << 5);
	buffer[ 6] = 0x00;
	buffer[ 7] = 0x00;
	// RXF2 - Buffer 1
	buffer[ 8] = (uint8_t)(RX_ID_1A >> 3);
	buffer[ 9] = (uint8_t)(RX_ID_1A << 5);
	buffer[10] = 0x00;
	buffer[11] = 0x00;
	can_write( RXF0SIDH, &buffer[0], 12 );

	// RXF3 - Buffer 1
	buffer[ 0] = (uint8_t)(RX_ID_1B >> 3);
	buffer[ 1] = (uint8_t)(RX_ID_1B << 5);
	buffer[ 2] = 0x00;
	buffer[ 3] = 0x00;
	// RXF4 - Buffer 1
	buffer[ 4] = (uint8_t)(RX_ID_1C >> 3);
	buffer[ 5] = (uint8_t)(RX_ID_1C << 5);
	buffer[ 6] = 0x00;
	buffer[ 7] = 0x00;
	// RXF5 - Buffer 1
	buffer[ 8] = (uint8_t)(RX_ID_1D >> 3);
	buffer[ 9] = (uint8_t)(RX_ID_1D << 5);
	buffer[10] = 0x00;
	buffer[11] = 0x00;
	can_write( RXF3SIDH, &buffer[0], 12 );

	// RXM0 - Buffer 0
	buffer[ 0] = (uint8_t)(RX_MASK_0 >> 3);
	buffer[ 1] = (uint8_t)(RX_MASK_0 << 5);
	buffer[ 2] = 0x00;
	buffer[ 3] = 0x00;
	// RXM1 - Buffer 1
	buffer[ 4] = (uint8_t)(RX_MASK_1 >> 3);
	buffer[ 5] = (uint8_t)(RX_MASK_1 << 5);
	buffer[ 6] = 0x00;
	buffer[ 7] = 0x00;
	can_write( RXM0SIDH, &buffer[0], 8 );

	// Switch out of config mode into normal operating mode
	can_mod( CANCTRL, 0xE0, 0x00 );			// CANCTRL register, modify upper 3 bits, mode = Normal

	CAN_BUSY = 0;
}

/**
 * Receives a CAN message from the MCP2515 in RXB0, and stores it into {@link canq_rx}
 *
 *	- Run this routine when an IRQ is received
 *	- Query the controller to identify the source of the IRQ
 *		- If it was an ERROR IRQ, read & clear the Error Flag register
 *		- If it was an RX IRQ, read the message and address
 *		- If both, handle the error preferentially to the message
 *	- Clear the appropriate IRQ flag bits
 */
void can_receive(void)
{
	uint8_t flags; //Bitfield for flags

	//Read out the interrupt flags register
	can_read(CANINTF, &flags, 1);

	//Check for errors
	if((flags & MCP_IRQ_ERR) != 0x00){
		// Read error flags and counters
		can_read( EFLAG, &buffer[0], 1 );
		can_read( TEC, &buffer[1], 2 );
		// Clear error flags
		can_mod( EFLAG, buffer[0], 0x00 );	// Modify (to '0') all bits that were set
		// Return error code, a blank address field, and error registers in data field
		can_rx_push_ptr->status = CAN_ERROR;
		can_rx_push_ptr->address = 0x0000;
		can_rx_push_ptr->data.data_u8[0] = flags;		// CANINTF
		can_rx_push_ptr->data.data_u8[1] = buffer[0];	// EFLG
		can_rx_push_ptr->data.data_u8[2] = buffer[1];	// TEC
		can_rx_push_ptr->data.data_u8[3] = buffer[2];	// REC

		//Push the message into the CAN receive queue
		can_push(CAN_RX);

		// Clear the IRQ flag
		can_mod( CANINTF, MCP_IRQ_ERR, 0x00 );
	}
	// No error, check for received messages, buffer 0
	else if(( flags & MCP_IRQ_RXB0 ) != 0x00 ){
		// Read in the info, address & message data
		can_read( RXB0CTRL, &buffer[0], 14 );
		// Fill out return structure
		// check for Remote Frame requests and indicate the status correctly
		if(( buffer[0] & MCP_RXB0_RTR ) == 0x00 ){
			// We've received a standard data packet
			can_rx_push_ptr->status = CAN_OK;
			// Fill in the data
			can_rx_push_ptr->data.data_u8[0] = buffer[ 6];
			can_rx_push_ptr->data.data_u8[1] = buffer[ 7];
			can_rx_push_ptr->data.data_u8[2] = buffer[ 8];
			can_rx_push_ptr->data.data_u8[3] = buffer[ 9];
			can_rx_push_ptr->data.data_u8[4] = buffer[10];
			can_rx_push_ptr->data.data_u8[5] = buffer[11];
			can_rx_push_ptr->data.data_u8[6] = buffer[12];
			can_rx_push_ptr->data.data_u8[7] = buffer[13];
		}
		else{
			// We've received a remote frame request
			// Data is irrelevant with an RTR
			can_rx_push_ptr->status = CAN_RTR;
		}
		// Fill in the address
		can_rx_push_ptr->address = buffer[1];
		can_rx_push_ptr->address = can_rx_push_ptr->address << 3;
		buffer[2] = buffer[2] >> 5;
		can_rx_push_ptr->address = can_rx_push_ptr->address | buffer[2];

		//Push the message into the receive queue
		can_push(CAN_RX);

		// Clear the IRQ flag
		can_mod( CANINTF, MCP_IRQ_RXB0, 0x00 );
	}
	else if(( flags & MCP_IRQ_RXB1 ) != 0x00 ){
		// Read in the info, address & message data
		can_read( RXB1CTRL, &buffer[0], 14 );
		// Fill out return structure
		// check for Remote Frame requests and indicate the status correctly
		if(( buffer[0] & MCP_RXB1_RTR ) == 0x00 ){
			// We've received a standard data packet
			can_rx_push_ptr->status = CAN_OK;
			// Fill in the data
			can_rx_push_ptr->data.data_u8[0] = buffer[ 6];
			can_rx_push_ptr->data.data_u8[1] = buffer[ 7];
			can_rx_push_ptr->data.data_u8[2] = buffer[ 8];
			can_rx_push_ptr->data.data_u8[3] = buffer[ 9];
			can_rx_push_ptr->data.data_u8[4] = buffer[10];
			can_rx_push_ptr->data.data_u8[5] = buffer[11];
			can_rx_push_ptr->data.data_u8[6] = buffer[12];
			can_rx_push_ptr->data.data_u8[7] = buffer[13];
		}
		else{
			// We've received a remote frame request
			// Data is irrelevant with an RTR
			can_rx_push_ptr->status = CAN_RTR;
		}
		// Fill in the address
		can_rx_push_ptr->address = buffer[1];
		can_rx_push_ptr->address = can_rx_push_ptr->address << 3;
		buffer[2] = buffer[2] >> 5;
		can_rx_push_ptr->address = can_rx_push_ptr->address | buffer[2];

		//Push the message into the receive queue
		can_push(CAN_RX);

		// Clear the IRQ flag
		can_mod( CANINTF, MCP_IRQ_RXB1, 0x00 );
	}
	// Check for wakeup events
	else if(( flags & MCP_IRQ_WAKE ) != 0x00 ){
		// Clear the IRQ flag
		can_mod( CANINTF, MCP_IRQ_WAKE, 0x00 );
	}
	// Else, spurious interrupt
}

/**
 * Query if the primary CAN transmit mailbox is full.
 * @return 1 if is busy, 0 if not
 */
int8_t can_isBusy(void)
{
	if((can_read_status() & 0x04) == 0x04)
	{
		return TRUE;
	} else return FALSE;
}

/**
 * Transmits a CAN message out of {@link canq_tx} to the bus.
 *
 *	- If there are packets in the Queue, pick out the next one and send it
 *	- Accepts address and data payload via can_interface structure
 *	- Uses status field to pass in DLC (length) code
 *	- Checks mailbox 1 is free, if not, returns -1 without transmitting packet
 *	- Busy waits while message is sent to CAN controller on SPI port
 *	- Only uses mailbox 1, to avoid corruption from CAN Module Errata (Microchip DS80179G)
 *
 * @return 1 = transmitted packet from the queue, -1 = all mailboxes busy, -3 = nothing to transmit
 */
int8_t can_transmit(void)
{
	//Check Queue
	if(can_tx_push_ptr != can_tx_pop_ptr){
		//Check for mailbox 1 busy
		if(can_isBusy() == 1) {
			return -1;
		}
		else {
			can_message* tx = can_pop(CAN_TX);

			//Format the data for MCP2515
			buffer[ 0] = (uint8_t)(tx->address >> 3);
			buffer[ 1] = (uint8_t)(tx->address << 5);
			buffer[ 2] = 0x00;											// Extended identifier bits, unused
			buffer[ 3] = 0x00;											// Extended identifier bits, unused
			buffer[ 4] = 8;												// Data length code, always sending 8-bit data.
			buffer[ 5] = tx->data.data_u8[0];
			buffer[ 6] = tx->data.data_u8[1];
			buffer[ 7] = tx->data.data_u8[2];
			buffer[ 8] = tx->data.data_u8[3];
			buffer[ 9] = tx->data.data_u8[4];
			buffer[10] = tx->data.data_u8[5];
			buffer[11] = tx->data.data_u8[6];
			buffer[12] = tx->data.data_u8[7];
			can_write_tx(&buffer[0]);
			can_rts(0);
			return 1; //Success
		}
	}
	else { //Nothing to transmit
		return -3;
	}
}

/**
 * Pops the next available element out of the specified CAN buffer.
 *
 * @param buffer One of the {@link CAN_BUFFERS}
 * @return A pointer to the next-in-line can_message object to read out of the receive queue
 */
can_message* can_pop(uint8_t buffer)
{
	can_message* ret;
	switch(buffer)
	{
		case CAN_RX:
			ret = can_rx_pop_ptr;	//Return the message we need to pop from the queue
			can_rx_pop_ptr++;		//Increment to the next message
			if(can_rx_pop_ptr == (canq_rx + CAN_BUF_LEN)) can_rx_pop_ptr = canq_rx;
			break;
		case CAN_TX:
			ret = can_tx_pop_ptr;	//Return the message we need to pop from the queue
			can_tx_pop_ptr++;		//Increment to the next message
			if(can_tx_pop_ptr == (canq_tx + CAN_BUF_LEN)) can_tx_pop_ptr = canq_tx;
			break;
	}
	return ret;
}

/**
 * Pushes a CAN message into the given CAN buffer.
 *
 * @param buffer One of the {@link CAN_BUFFERS}
 */
void can_push(uint8_t buffer)
{
	switch(buffer)
	{
		case CAN_RX:
			can_rx_push_ptr++;
			if(can_rx_push_ptr == (canq_rx+CAN_BUF_LEN)) can_rx_push_ptr = canq_rx;
			break;
		case CAN_TX:
			can_tx_push_ptr++;
			if(can_tx_push_ptr == (canq_tx+CAN_BUF_LEN)) can_tx_push_ptr = canq_tx;
			break;
	}
}

/**
 * Put the MCP2515 into sleep mode
 */
void can_sleep(void)
{
	uint8_t status;

	//Sing the MCP2515 a lullaby
	can_mod(CANCTRL, 0xE0, 0x20); //CANCTRL register, modify upper 3 bits, mode = Sleep

	//Wait until it falls asleep
	while((status & 0xE0) != 0x20) {
		can_read(CANSTAT, &status, 1);
	}
}

/**
 * Wake the MCP2515 from sleep.
 */
void can_wake(void)
{
	//Wake it up
	can_mod(CANCTRL, 0xE0, 0x00); //CANCTRL, upper 3 bits, mode=Normal
}



//Private function implementations

/**
 * Resets MCP2515 CAN controller via SPI port.
 *	- SPI port must be already initialised
 */
void can_reset(void)
{
	can_select;
	spi_tx(REG_BUS, MCP_RESET);
	can_deselect;
}

/**
 * Reads data bytes from the MCP2515.
 * @param address Starting address to read from in chip
 * @param ptr Array of bytes for return data
 * @param bytes Number of bytes to read
 */
void can_read(uint8_t address, uint8_t *ptr, uint8_t bytes)
{
	uint8_t i;

	can_select;
	spi_tx(REG_BUS, MCP_READ);
	spi_tx(REG_BUS, address);
	for(i = 0; i < bytes; i++) *ptr++ = spi_tx(REG_BUS, 0x00);
	can_deselect;
}

/**
 * Writes data bytes to the MCP2515.
 * @param address Starting address to write to in the chip
 * @param ptr Pointer to array of bytes to write
 * @param bytes Number of bytes to write
 */
void can_write(uint8_t address, uint8_t *ptr, uint8_t bytes)
{
	uint8_t i;

	can_select;
	spi_tx(REG_BUS, MCP_WRITE);
	spi_tx(REG_BUS, address);
	for(i = 0; i < (bytes-1); i++){
		spi_tx(REG_BUS, *ptr++);
	}
	spi_tx(REG_BUS, *ptr);
	can_deselect;
}

/**
 * Writes data bytes to transmit buffer 0 (TXB0) in MCP2515.
 *
 * @note This can be extended to support multiple transmit buffers by adding another parameter that
 * simply takes a uint8_t with a 0, 1, or 2 in it marking which transmit buffer to send to, and then
 * in the initial spi_tx bitwise OR MCP_WRITE_TX with the new buffer parameter.
 *
 * @param ptr A 13-byte long array of data to write to the transmit buffer.
 *	- Pass in buffer number and start position as defined in MCP2515 datasheet
 */
void can_write_tx(uint8_t *ptr)
{
	uint8_t i;

	can_select;
	spi_tx(REG_BUS, MCP_WRITE_TX);
	for( i = 0; i < 13; i++ ){
		spi_tx(REG_BUS, *ptr++);
	}
	can_deselect;
}

/**
 * Request to send selected transmit buffer.
 *
 * @param address Which buffer to transmit, 0, 1, or 2
 */
void can_rts(uint8_t address)
{
	uint8_t i;

	// Set up address bits in command byte
	i = MCP_RTS;
	if(address == 0) i |= 0x01;
	else if(address == 1) i |= 0x02;
	else if(address == 2) i |= 0x04;

	// Write command
	can_select;
	spi_tx(REG_BUS, i);
	can_deselect;
}

/**
 * Reads MCP2515 status register
 *
 * @return The value of the status register
 */
uint8_t can_read_status(void)
{
	uint8_t status;

	can_select;
	spi_tx(REG_BUS, MCP_STATUS);
	status = spi_tx(REG_BUS, 0x00);
	can_deselect;
	return status;
}

/**
 * Reads MCP2515 RX status (filter match) register
 *
 * @return MCP2515 filter match register
 */
uint8_t can_read_filter(void)
{
	uint8_t status;

	can_select;
	spi_tx(REG_BUS, MCP_FILTER);
	status = spi_tx(REG_BUS, 0x00);
	can_deselect;
	return status;
}

/**
 * Modifies selected register in MCP2515.
 *
 * @param address Register to be modified
 * @param mask Bit mask
 * @param data New bit data
 */
void can_mod(uint8_t address, uint8_t mask, uint8_t data)
{
	can_select;
	spi_tx(REG_BUS, MCP_MODIFY);
	spi_tx(REG_BUS, address);
	spi_tx(REG_BUS, mask);
	spi_tx(REG_BUS, data);
	can_deselect;
}
