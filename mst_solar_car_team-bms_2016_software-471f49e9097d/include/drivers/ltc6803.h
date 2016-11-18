/**
 * @file ltc6803.h
 *
 * @author Jesse Cureton
 */
#ifndef LTC6803_H_
#define LTC6803_H_

//Macros for various things
#define LTC_SEL			P8OUT &= ~LTC_CS	///< Select the LTC6803 stack over SPI
#define LTC_DESEL		P8OUT |= LTC_CS		///< Deselect the LTC6803 stack over SPI
#define RESET_PEC		pecVAL = 0x41		///< Reset the PEC value to its initial value, required call before calling {@link ltc6803PEC()}

/**
 * @defgroup LTC6803_CDC_GROUP LTC6803 CDC values
 *
 * Comparator duty cycle values for the LTC6803.
 *
 * These values are defined on pg. 24 of the <a href="http://cds.linear.com/docs/en/datasheet/680313fa.pdf">datasheet</a>
 *
 * @{
 */
#define CDC0			0x00	///< Standby mode. No measurements taken.
#define CDC1			0x01	///< Comparator off. Measurements taken only when requested by SPI master. A measurement takes 13ms in this mode.
/** @} */

/**
 * This is the global-scope cell array.
 *
 * Data is indexed as cells[{@link BATTERY_BANKS} value][{@link BATTERY_CELLS} value].
 * The single source of truth for anything with regard to a cell voltage, bank, or resistance. */
cell_t cells[NUM_BANKS+1][12];

/**
 * The most-recent running packet error correction value.
 *
 * @see ltc6803PEC()
 */
uint8_t	pecVAL;
/**
 * The temporary storage variable for packet error correction.
 *
 * @see ltc6803PEC()
 */
uint8_t pecIN;

//Control Functions
void ltc6803_write_config(uint8_t cdcval);
void ltc6803PEC(uint8_t val);
void ltc6803Conv();
void ltc6803ReadIn();

//Configuration Constants
const uint8_t CFGR[6] = {	0x18,	//CFGR0 - Level polling, 10 cell mode, stay in standby until a conversion
							0x00,	//CFGR1 - No cell balancing
							0x00, 	//CFGR2 - Don't mask 1-4, no cell balancing
							0xF0,	//CFGR3 - Mask the top four cells, 12, 11, 10, 9
							0x00,	//CFGR4 - Don't care
							0x00  };//CFGR5 - Don't care

/**
 * @defgroup LTC6803_CFG_REGS LTC6803 Config registers
 *
 * Addresses for the configuration registers of the LTC6803.
 *
 * These values are defined beginning on pg. 23 of the <a href="http://cds.linear.com/docs/en/datasheet/680313fa.pdf">datasheet</a>
 *
 * @{
 */
#define WRCFG				0x01	///< Write configuration register group
#define RDCFG				0x02	///< Read configuration register group
#define RDCV				0x04	///< Read all cell voltages
#define	RDFLG				0x0C	///< Read flag register group
#define	STCVAD				0x10	///< Start cell voltage conversion - all cells
#define STCVDC				0x60	///< Start cell voltage conversion - discharge enabled
#define STCCLR				0x1D	///< Clear cell voltage registers
#define PLADC				0x40	///< Poll ADC status
#define	PLINT				0x50	///< Poll interrupt status
#define DAGN				0x52	///< Start diagnose and poll status
#define RDDGNR				0x54	///< Read diagnostic register
/** @} */

/**
 * @defgroup LTC6803_CMD_PECS LTC6803 calculated PEC values
 *
 * Precalculated packet error correction values for common LTC6803 commands.
 *
 * These values are defined beginning on pg. 21 of the <a href="http://cds.linear.com/docs/en/datasheet/680313fa.pdf">datasheet</a>
 *
 * @{
 */
#define WRCFG_PEC			0xC7	///< Write configuration register group
#define RDCFG_PEC			0xCE	///< Read configuration register group
#define RDCV_PEC			0xDC	///< Read all cell voltages
#define	RDFLG_PEC			0xE4	///< Read flag register group
#define	STCVAD_PEC			0xB0	///< Start cell voltage conversion - all cells
#define STCVDC_PEC			0xE7	///< Start cell voltage conversion - discharge enabled
#define STCCLR_PEC			0x93    ///< Clear cell votlage registers
#define PLADC_PEC			0x07	///< Poll ADC status
#define	PLINT_PEC			0x77	///< Poll interrupt status
#define DAGN_PEC			0x79	///< Start diagnose and poll status
#define RDDGNR_PEC			0x6B	///< Read diagnostic register
/** @} */

#endif
