/**
 * @file mcp23s.c
 *
 * Dual-role driver for the Microchip MCP23Sxx line of SPI-based IO expanders.
 *
 * In particular, this driver implements support for the <a href="http://www.microchip.com/wwwproducts/en/MCP23S08">MCP23S08</a>
 * and <a href="http://www.microchip.com/wwwproducts/en/MCP23S17">MCP23S17</a>, which are functionally identical, however the MCP23S17 adds a second port and all
 * relevant registers, for 16 GPIOs instead of 8 on the MCP23S08.
 *
 * @author Jesse Cureton
 */
#include "msp430.h"
#include "main.h"
#include "datatypes.h"
#include "pins.h"
#include "spi.h"

#include "drivers/mcp23s.h"

/**
 * Initialize both MCP23Sxx IO expanders on the board.
 *
 * TODO: This should be parameterized to supportinitializing different chips.
 */
void mcp23s_init()
{
	//Because of a probable chip errata on the MCP23S** we have to init, write some data, then init again.
	mcp23sWrite(IO_LEDS, IOCONA, 0xA8);
	mcp23sWrite(IO_LEDS, IODIRA, 0x00);
	mcp23sWrite(IO_LEDS, IODIRB, 0x00);

	mcp23sWrite(IO_OPENDRAINS, IOCONA, 0xA8);
	mcp23sWrite(IO_OPENDRAINS, IODIRA, 0x00);

	mcp23sWrite(IO_LEDS, REG_STAT_LEDS, 0xFF);
	mcp23sWrite(IO_LEDS, REG_ERR_LEDS, 0xFF);
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, 0xFF);

	mcp23sWrite(IO_LEDS, IOCONA, 0xA8);
	mcp23sWrite(IO_LEDS, IODIRA, 0x00);
	mcp23sWrite(IO_LEDS, IODIRB, 0x00);

	mcp23sWrite(IO_OPENDRAINS, IOCONA, 0xA8);
	mcp23sWrite(IO_OPENDRAINS, IODIRA, 0x00);

	mcp23sWrite(IO_LEDS, REG_ERR_LEDS, ERRORS);
	mcp23sWrite(IO_LEDS, REG_STAT_LEDS, STATUS);
	mcp23sWrite(IO_OPENDRAINS, REG_OD_CONNS, 0x00);
}

/**
 * Write a value to a given register in a given IO expander.
 *
 * @param chip The IO expander to write to - one of {@link IO_EXPS}.
 * @param reg The register to read from on the chip, one of {@link MCP23S_REGS} or {@link MCP23S_OUTPUTS}.
 * @param val The value to write to the register.
 */
void mcp23sWrite(uint8_t chip, uint8_t reg, uint8_t val)
{
	chip == IO_LEDS ? LED_SEL : OPENDRAIN_SEL;
	spi_tx(REG_BUS, ADDR_W);
	spi_tx(REG_BUS, reg);
	spi_tx(REG_BUS, val);
	chip == IO_LEDS ? LED_DESEL : OPENDRAIN_DESEL;
	return;
}

/**
 * Read a value from a given register in a given IO expander.
 *
 * @param chip The IO expander to read from - one of {@link IO_EXPS}.
 * @param reg The register to read from on the chip, one of {@link MCP23S_REGS} or {@link MCP23S_OUTPUTS}.
 * @return The value contained in the requested register on the requested chip.
 */
uint8_t mcp23sRead(uint8_t chip, uint8_t reg)
{
	uint8_t val;
	chip == IO_LEDS ? LED_SEL : OPENDRAIN_SEL;
	spi_tx(REG_BUS, ADDR_R);
	spi_tx(REG_BUS, reg);
	val = spi_tx(REG_BUS, 0x00);
	chip == IO_LEDS ? LED_DESEL : OPENDRAIN_DESEL;
	return val;
}
