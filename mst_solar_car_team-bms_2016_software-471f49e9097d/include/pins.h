/**
 * @file pins.h
 *
 * A description of all microcontroller pins used by the project.
 *
 * @author Jesse Cureton
 */

#ifndef PINS_H_
#define PINS_H_

//SPI Chip selects
#define OPENDRAIN_CS	BIT3	///< Chip-select for the {@link mcp23s.c MCP23S08} used to control the open-drain outputs. Pin 2.3
#define LED_CS			BIT7	///< Chip-select for the {@link mcp23s.c MCP23S17} used to control the LEDs. Pin 3.7
#define CAN_CS 			BIT1	///< Chip-select for the {@link can.c MCP2515} CAN controller. Pin 8.1
#define LTC_CS 			BIT2	///< Chip-select for the {@link ltc6803.c LTC6803} battery stack monitors. Pin 8.2

//SPI Pins
#define	MOSI			BIT0	// 3.0
#define	MISO			BIT1	// 3.1
#define	SCK				BIT2	// 3.2
#define MOSI_2			BIT3	// 3.3
#define MISO_2			BIT4	// 3.4
#define SCK_2			BIT7	// 2.7

//Analog Inputs
#define PACK_VOLTAGE	BIT0	///< ADC12 channel 0, used to measure pack voltage. Pin 6.0
#define	PACK_CURRENT	BIT1	///< ADC12 channel 1, used to measure pack current sensor. Pin 6.1
#define AUX_VOLTAGE		BIT2	///< ADC12 channel 2, used to measure aux pack voltage. Pin 6.2

//1-Wire Pins
#define BANK1_TEMP		BIT2	///< The {@link onewire.c 1-wire} data pin for bank 1 temperature sensors. Pin 4.2
#define BANK2_TEMP		BIT6	///< The {@link onewire.c 1-wire} data pin for bank 2 temperature sensors. Pin 6.6
#define BANK3_TEMP		BIT6	///< The {@link onewire.c 1-wire} data pin for bank 3 temperature sensors. Pin 1.6

//GPIO
#define CAN_INT			BIT2	///< The CAN interrupt pin. Pin 1.2
#define TEMP3_MOSFET	BIT3 	///< The active-low gate pin of the strong pull-up MOSFET for parasitic temperature sensor power. Pin 1.3
#define TEMP2_MOSFET	BIT4	///< The active-low gate pin of the strong pull-up MOSFET for parasitic temperature sensor power. Pin 1.4
#define TEMP1_MOSFET	BIT5	///< The active-low gate pin of the strong pull-up MOSFET for parasitic temperature sensor power. Pin 1.5

#endif /* PINS_H_ */
