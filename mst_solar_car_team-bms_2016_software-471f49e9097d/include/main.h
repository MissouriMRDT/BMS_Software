/**
 * @file main.h
 *
 * @author Jesse Cureton
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "datatypes.h"
#include <msp430.h>

/**
 * @defgroup SYSTEM_STATES System states
 *
 * Available global states for the main loop state machine.
 *
 * @{
 */
#define STATE_IDLE		0x00	///< Housekeeping state where items needed for all states are handled. All states end by returning to state_idle.
#define STATE_ISOLATED	0x01	///< This state is entered whenever the pack is isolated due to a fault.
#define STATE_MEASURE	0x02	///< Primary system state where all measurements occur.
#define STATE_BOOTUP	0x03	///< Initial boot state to complete early initialization and measurements.
/** @} */
volatile uint8_t state;			///< Current global system state, one of the {@link SYSTEM_STATES} values.

//Functions
void 	clock_init();
void 	io_init();
void 	timer_init();
void	processErrors();
void 	processADCValues();
void	processVoltages();
void	processTemperatures();
void	processCANMessages();
void	sendCANMessages();
void	serviceCANReception();
void	updateFanSpeed();

//Global definitions
#define MCLK_FREQ	20000000	///< Master clock frequency in Hz

/**
 * @defgroup ADC_INDEXES ADC Measurement IDs
 *
 * ID Definitions for ADC measurement indexing
 *
 * @{
 */
#define ADC_PACK_VOLTAGE	0	///< Total pack voltage measurement
#define ADC_PACK_CURRENT 	1	///< Pack current measurement
#define ADC_AUX_VOLTAGE  	2	///< Aux pack voltage measurement
/** @} */

/**
 * @defgroup BATTERY_BANKS Battery Bank IDs
 *
 * ID Definitions for battery banks
 *
 * @{
 */
#define BANK1	 	0	///< Bank 1
#define BANK2	 	1   ///< Bank 2
#define BANK3 		2   ///< Bank 3
/** @} */

/**
 * @defgroup BATTERY_CELLS Battery Cell IDs
 *
 * ID Definitions for battery cells
 *
 * @{
 */
#define CELL1		0	///< Cell 1
#define CELL2		1	///< Cell 2
#define CELL3		2	///< Cell 3
#define CELL4		3	///< Cell 4
#define CELL5		4	///< Cell 5
#define CELL6		5	///< Cell 6
#define CELL7		6	///< Cell 7
#define CELL8		7	///< Cell 8
#define CELL9		8	///< Cell 9
#define CELL10		9	///< Cell 10
#define CELL11		10	///< Cell 11
#define CELL12		11	///< Cell 12
/** @} */

//////////////////////
//  System Macros
//////////////////////
#define delayMicros(x) __delay_cycles(x * (MCLK_FREQ / 1000000))	///< Delay for x microseconds
#define delayMillis(x) __delay_cycles(x * (MCLK_FREQ / 1000))		///< Delay for x milliseconds

/**
 * @defgroup SYSTEM_ERRORS System error bitfield definitions
 *
 * These define the proper bits in the {@link ERRORS} and {@link WARNINGS} bitfields for certain system conditions.
 *
 * @{
 */
#define NO_ERROR		0x00    ///< No current errors or warnings.
#define ESTOP_ERROR		BIT8	///< We are tripped because of an estop
#define UNKNOWN_ERROR 	BIT7 	///< This isn't actually triggered anywhere, but just in case we need to declare an error we aren't sure the cause of.
#define TEMP_FAULT		BIT6	///< Temperature value out of bounds - always isolate
#define OV_FAULT		BIT5	///< Overvoltage fault
#define UV_FAULT		BIT4	///< Undervoltage fault
#define CHARGE_FAULT	BIT3	///< Charge current fault
#define DISCHARGE_FAULT BIT2	///< Discharge current fault
#define AUX_FAULT		BIT1	///< Aux pack voltage too low
#define COMMS_FAULT		BIT0	///< Generic communication fault - Probably LTC6803 communication
/** @} */
volatile uint16_t ERRORS;		///< The global-scope errors bitfield, bits correspond to {@link SYSTEM_ERRORS} values.
volatile uint8_t WARNINGS;		///< The global-scope warnings bitfield, bits correspond to {@link SYSTEM_ERRORS} values.

/**
 * @defgroup SYSTEM_STATUS System status bitfield definitions
 *
 * These define the proper bits in the {@link STATUS} bitfield for certain system conditions.
 *
 * @{
 */
#define STAT_POWER		BIT0	///< Set if the board is powered, off if not.
#define STAT_CAN		BIT1	///< Set if CAN communication is currently working, cleared if it is down.
#define STAT_TRIPPED	BIT2	///< Set if the battery pack is isolated due to any fault.
#define STAT_CHARGE		BIT3	///< Set if charge mode is enabled.
#define STAT_GEN3		BIT4	///< Unused.
#define STAT_GEN4		BIT5	///< Unused.
#define STAT_GEN5		BIT6	///< Unused.
#define STAT_GEN6		BIT7	///< Unused.
/** @} */
volatile uint8_t STATUS;		///< The global-scope status bitfield, bits correspond to {@link SYSTEM_STATUS}

/**
 * @defgroup OPENDRAIN_DEFS Open drain output definitions
 *
 * These identify the peripherals connected to each of our 8 open-drain outputs, and denote their proper bits in {@link OD_OUTPUTS}
 *
 * @{
 */
#define OD_NO_OUTPUTS	0x00	///< All opendrains are disabled.
#define OD_COMMON_GND	BIT0	///< Common ground relay
#define OD_PRECHARGE	BIT1	///< Precharge relay
#define OD_MOTOR		BIT2	///< Motor relay
#define OD_ARRAY		BIT3	///< Array relay
#define OD_FAN1			BIT4	///< Fan 1
#define OD_FAN2			BIT5	///< Fan 2
#define OD_FAN3			BIT6	///< Fan 3
#define OD_FAN4			BIT7	///< Fan 4
/** @} */
volatile uint8_t OD_OUTPUTS;	///< The global-scope bitfield containing the values in the open-drain output register, bits correspond to {@link OPENDRAIN_DEFS}.

/**
 * @defgroup ESTOP_INDICES Emergency Stop Indexes
 *
 * These identify the estop count indexes for the {@link countSinceValidEStop} tracker.
 *
 * @{
 */
#define ESTOP_UBCB		0	///< Upper body control board estop
#define ESTOP_DE		1	///< Driver electronics estop
/** @} */
#define NUM_ESTOPS		2	///< The number of estop packets we are expecting
volatile uint8_t countSinceValidEStop[NUM_ESTOPS];	///< The global-scope array for tracking the number of measurement loops since we received a valid estop packet. Indexed as one of the {@link ESTOP_INDICES}.

/////////////////////
// System Utility Functions
/////////////////////
void		shiftAndAverageReading(float* array, float valueToAdd);
float 		getSummedPackVoltage();
float		getPackTemp();
cell_t* 	getMinCell();
cell_t* 	getMaxCell();
temp_t*		getMinTemp();
temp_t*		getMaxTemp();


#endif /* MAIN_H_ */
