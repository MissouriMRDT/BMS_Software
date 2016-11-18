/**
 * @file mcp23s.h
 *
 * Dual-role driver for the Microchip MCP23Sxx line of SPI-based IO expanders.
 *
 * In particular, this driver implements support for the <a href="http://www.microchip.com/wwwproducts/en/MCP23S08">MCP23S08</a>
 * and <a href="http://www.microchip.com/wwwproducts/en/MCP23S17">MCP23S17</a>, which are functionally identical, however the MCP23S17 adds a second port and all
 * relevant registers, for 16 GPIOs instead of 8 on the MCP23S08.
 *
 * @author Jesse Cureton
 */
#ifndef MCP23S_H_
#define MCP23S_H_

/**
 * @defgroup IO_EXPS IO Expanders
 *
 * Available IO expanders on the board.
 *
 * @{
 */
#define IO_LEDS			0x00	///< 16-bit MCP23S17 IO expander for LED outputs
#define IO_OPENDRAINS	0x01	///< 8-bit MCP23S08 IO expander for open-drain outputs
/** @} */

//Address bits
#define ADDR_R			0x41	//MCP23S read address
#define ADDR_W			0x40	//MCP23S write address

//Macros
#define LED_SEL			P3OUT &= ~LED_CS		///< Select the IO expander connected to the LEDs over SPI
#define LED_DESEL	  	P3OUT |= LED_CS			///< Deselect the IO expander connected to the LEDs over SPI
#define OPENDRAIN_SEL	P2OUT &= ~OPENDRAIN_CS	///< Select the IO expander connected to the open drain outputs over SPI
#define OPENDRAIN_DESEL	P2OUT |= OPENDRAIN_CS	///< Deselect the IO expander connected to the open drain outputs over SPI

//Functions
void mcp23s_init();
void mcp23sWrite(uint8_t chip, uint8_t reg, uint8_t val);
uint8_t mcp23sRead(uint8_t chip, uint8_t reg);

/**
 * @defgroup MCP23S_OUTPUTS MCP23Sxx board-specific outputs
 *
 * These denote what the output latch registers correspond to on the board.
 *
 * @{
 */
#define REG_ERR_LEDS	OLATA	///< Error LEDs
#define REG_STAT_LEDS	OLATB	///< Status LEDs
#define REG_OD_CONNS	OLATA	///< Open-drain connectors
/** @} */

//Open Drain definitions
#define RELAY_BITS	    0xF8

/**
 * @defgroup MCP23S_REGS MCP23Sxx registers
 *
 * Internal registers for the MCP23Sxx chips.
 *
 * @{
 */
#define IODIRA			0x00	///< IO direction A
#define IPOLA			0x01	///< Input polarity A
#define GPINTENA		0x02	///< Interrupt enable A
#define DEFVALA			0x03	///< Default comparison for interrupt A
#define INTCONA			0x04	///< Interrupt configuration A
#define IOCONA			0x05	///< Configuration register A
#define GPPUA			0x06	///< Pull up enable A
#define INTFA			0x07	///< Interrupt flag register A
#define INTCAPA			0x08	///< Interrupt capture register A
#define GPIOA			0x09	///< Values on Port A
#define OLATA			0x0A	///< Values written to Port A

#define IODIRB			0x10	///< IO direction B, only on MCP23S17
#define IPOLB			0x11	///< Input polarity B, only on MCP23S17
#define GPINTENB		0x12	///< Interrupt enable B, only on MCP23S17
#define DEFVALB			0x13	///< Default comparison for interrupt B, only on MCP23S17
#define INTCONB			0x14	///< Interrupt configuration B, only on MCP23S17
#define IOCONB			0x15	///< Configuration register B, only on MCP23S17
#define GPPUB			0x16	///< Pull up enable B, only on MCP23S17
#define INTFB			0x17	///< Interrupt flag register B, only on MCP23S17
#define INTAPB			0x18	///< Interrupt capture register B, only on MCP23S17
#define GPIOB			0x19	///< Values on Port B, only on MCP23S17
#define OLATB			0x1A	///< Values written to Port B, only on MCP23S17
/** @} */

#endif
