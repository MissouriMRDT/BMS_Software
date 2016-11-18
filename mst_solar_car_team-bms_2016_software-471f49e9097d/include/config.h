/**
 * @file config.h
 *
 * @date Aug 14, 2015
 * @author Jesse Cureton
 */

#ifndef INCLUDE_CONFIG_H_
#define INCLUDE_CONFIG_H_

#include "main.h"

//////////////////////
// BMS Config Values
//////////////////////
#define BMS_BASE				0x300			///< CAN base address

#define PRECHARGE_DELAY			1000			///< Delay from precharge to output changeover, in ms
#define AVERAGE_PERIOD			5				///< Number of samples to average for acting on an event

#define FAULT_CURRENT_CHARGE	(float) -26.0	///< Charge current trip point. We are limited to 26A charge current on the NCR18650B cells
#define FAULT_CURRENT_DISCHARGE (float)  75.0	///< Discharge current trip point. We are limited to 75A discharge current on the NCR18650B cells

#define FAULT_VOLTAGE_MAX		(float) 4.2		///< Maximum cell voltage trip point.
#define FAULT_VOLTAGE_MIN		(float) 2.5		///< Minimum cell voltage trip point.
#define WARN_VOLTAGE_MAX		(float) 4.15	///< Warning point for high cell voltage.
#define WARN_VOLTAGE_MIN		(float) 2.55	///< Warning point for low cell voltage.
#define AUX_WARN_MIN			(float) 10.2	///< Warning point for low aux pack voltage

#define FAULT_TEMP_CHARGING		(float) 45		///< Fault temperature charging.
#define FAULT_TEMP_DISCHARGING	(float) 60		///< Fault temperature discharging.
#define WARN_TEMP_CHARGING		(float) 44.5	///< Maximum temperature to charge at. Beyond this point, array and regeneration must be disabled.
#define WARN_TEMP_DISCHARGING	(float) 58		///< Maximum temperature to discharge at. Beyond this point, the pack must be isolated
#define MIN_TEMP				(float)	-20		///< Minimum charge/discharge temperature. As though we'll ever hit this...

#define NUM_BANKS				BANK3			///< The number of banks in the pack. This should be configured as the {@link BATTERY_BANKS bank ID} of the most-positive bank in the pack
const uint8_t NUM_CELLS[3] = {	CELL10,			///< The {@link BATTERY_CELLS cell ID} of the highest potential cell in each bank, respectively. 0 if no cells in bank.
								CELL8,
								CELL8		};

#endif /* INCLUDE_CONFIG_H_ */
