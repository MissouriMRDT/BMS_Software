/**
 * @file main.c
 *
 * Battery management system logic for the 2016 battery management system for the Missouri S&T Solar Car Team.
 *
 * @date Mar 29, 2015
 * @author Jesse Cureton
 * @author Caleb Olson
 */

#include <msp430.h>
#include "main.h"
#include "config.h"
#include "pins.h"
#include "spi.h"
#include "adc.h"
#include "onewire.h"
#include "can.h"
#include "relays.h"
#include "fans.h"

#include "drivers/ds18b20.h"
#include "drivers/ltc6803.h"
#include "drivers/mcp23s.h"

uint64_t timeSinceChargeDisable = 0;	// Count how many iterations its been since we turned off charging

/**
 * Handles initialization of all peripherals before jumping to the state machine infinite loop.
 *
 * The state machine is based on the states contained in {@link SYSTEM_STATES}.
 *
 * @see SYSTEM_STATES
 *
 * @return Never actually ends, so never returns..
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;		// Stop the watchdog timer

    ERRORS 	 = NO_ERROR;
    WARNINGS = NO_ERROR;
    STATUS 	 = STAT_POWER;
    OD_OUTPUTS = OD_NO_OUTPUTS;

    clock_init();
    io_init();
    timer_init();

    spi_init(LTC_BUS);
    spi_init(REG_BUS);
    adc_init();
    can_init();
    mcp23s_init();
    ds18b20_init();
    ltc6803_write_config(CDC0);		//Init the LTC6803s to standby comparator duty cycle

    isolatePack();					//Start isolated
    state = STATE_BOOTUP;

    setFanSpeed(NUM_FANS, FAN_SPEED_1);	//Must always have at least 20% fan speed

    __enable_interrupt();  			// Enable interrupts globally

    uint8_t first_run = TRUE;
    uint8_t firstrunCount = 0;

    while(1)
    {
    	uint8_t rxcount;
    	rxcount = 0;
    	while(!(P1IN & CAN_INT))
		{
    		can_receive();
    		if(rxcount == 255)
    			break;
    		else rxcount++;
		}

    	switch(state)
    	{
			case STATE_IDLE:
				//////////////////////
				// Update IO expanders
				//   - Open drains & LEDS
				//////////////////////
				mcp23sWrite(IO_LEDS, REG_ERR_LEDS, ERRORS);
				mcp23sWrite(IO_LEDS, REG_STAT_LEDS, STATUS);

				sendCANMessages();		//Send any can messages pushed to the queue

				serviceCANReception();	//Process any CAN messages received in this cycle

				processErrors();		//Process errors from the current round of measurements

				serviceRelays();		//Finally account for any relay changes based on these measurements or received messages

				if(!first_run)
				{
					updateFanSpeed();		//Recalculate fan speed based on temperatures
					serviceFanPWM();		//Update the fan pwm counters
				} else if(firstrunCount == 5) {
					first_run = FALSE;
				} else firstrunCount++;

				__bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable interrupts
				break;

			case STATE_ISOLATED:
				//Pack is isolated, but we still continue processing data
				processADCValues();
				processVoltages();
				processTemperatures();
				processCANMessages();

				state = STATE_IDLE;			//Return to an idle state, but pack will stay isolated
				break;

			case STATE_MEASURE:
				processADCValues();
				processVoltages();
				processTemperatures();		//Read temperatures and set any error flags that occur because of it
				processCANMessages();		//Push all CAN messages into the transmit queue, and then transmit them all

				state = STATE_IDLE;			//Deal with any faults from the new measurements we've just taken
				break;

			case STATE_BOOTUP:
				//Collect enough samples to seed our average, and verify all is good at boot
				for(uint8_t i = 0; i < (2 * AVERAGE_PERIOD); i++)
				{
					processTemperatures();
					processADCValues();
					processVoltages();
					delayMillis(200);
				}

				//Initialize the estop missing counter
				for(uint8_t i = 0; i < NUM_ESTOPS; i++)
				{
					countSinceValidEStop[i] = 0;
				}

				ERRORS = NO_ERROR;

				state = STATE_IDLE;

				break;
    	}
    }
}

/**
 * Looks at current max temperature values and then sets fan speed based on that.
 */
void updateFanSpeed()
{
	temp_t* maxTemp = getMaxTemp();

	if(maxTemp->temperature[AVERAGE_PERIOD] < 30) setFanSpeed(NUM_FANS, FAN_SPEED_1);
	else if(maxTemp->temperature[AVERAGE_PERIOD] < 35) setFanSpeed(NUM_FANS, FAN_SPEED_2);
	else if(maxTemp->temperature[AVERAGE_PERIOD] < 40) setFanSpeed(NUM_FANS, FAN_SPEED_3);
	else setFanSpeed(NUM_FANS, FAN_SPEED_4);
}

/**
 * Deal with any errors during a measurement loop.
 *
 * Will check if any errors were encountered during a measurement loop, then isolate the pack
 * if there were.
 *
 * @return Proper next state for the system - either idle and go to sleep, or go to an isolated state
 */
void processErrors()
{
	for(uint8_t i = 0; i < NUM_ESTOPS; i++)
	{
		if(countSinceValidEStop[i] == 10)	//10*200ms = 2 second delay before we go offline due to missed packets
		{
			ERRORS |= ESTOP_ERROR;
		} else
		{
			countSinceValidEStop[i]++;
		}
	}

	if(ERRORS != 0)	//If there are any faults, isolate immediately
	{
		STATUS |= STAT_TRIPPED;
		STATUS &= ~STAT_CHARGE;
		isolatePack();
	}

	if(  !(WARNINGS & OV_FAULT) 								//Check no voltage warnings
	  && !(ERRORS & UV_FAULT)     && !(ERRORS & OV_FAULT)		//check no voltage faults
	  && !(ERRORS & TEMP_FAULT)									//Check no temp errors/warnings
	{
		//No errors, so charging is enabled.
		STATUS |= STAT_CHARGE;
		timeSinceDisableCharing = 0;
	} else {
		//Some kind of fault, disabling charging
		STATUS &= ~STAT_CHARGE;
		timeSinceDisableCharging++;
	}
}

/**
 * Send any CAN messages that have been pushed into the {@link canq_tx}.
 *
 * Will loop transmit until there are no messages left to transmit, along with verify CAN transmission
 * errors and update the {@link STATUS} bitfield accordingly.
 */
void sendCANMessages()
{
	CAN_BUSY = 1;
	//Check if we're somehow asleep, and if we are, wake up
	uint8_t canstat;
	can_read(CANSTAT, &canstat, 1);
	if((canstat & 0x20) == 0x20)
	{
		// Clear the IRQ flag for sleep
		// This should really probably never be triggered
		can_mod( CANINTF, MCP_IRQ_WAKE, 0x00 );
		can_wake();
	}

	while(can_tx_push_ptr != can_tx_pop_ptr)
	{
		int8_t ret = can_transmit();

		uint16_t count = 0;
		do
		{
			//Wait until the current message is sent, but bail out if we fail
			count++;
		}while(can_isBusy() && count < 1000);

		if(ret == -1 || count >= 1000)
		{
			// CAN communication is offline
			// We know because there are messages in the queue, but sending returns an error
			// because the last message we tried to send is still filling the mailbox
			STATUS &= ~STAT_CAN;
			break;
		} else if(ret == 1 && !(STATUS & STAT_CAN))
		{
			STATUS |= STAT_CAN;
		}
	}
	CAN_BUSY = 0;
}

/**
 * Service any messages received in the CAN queue, {@link canq_rx}.
 */
void serviceCANReception()
{
	// Loop through every received message
	while(can_rx_push_ptr != can_rx_pop_ptr)
	{
		can_message *msg = can_pop(CAN_RX);
		switch(msg->address)
		{
			//Emergency stop check
			case GLOBAL_ESTOP:
				if(msg->data.data_u8[6] & BIT4)			//UBCB estop byte
				{
					//If BIT4 is set, we know that this is a message from the UBCB
					if(!(msg->data.data_u8[6] & BIT0))	//Estop is triggered
					{
						isolatePack();
						ERRORS |= ESTOP_ERROR;
					}
					countSinceValidEStop[ESTOP_UBCB] = 0;
				}
				if(msg->data.data_u8[7] & BIT4)	//DE estop byte
				{
					if(!(msg->data.data_u8[7] & BIT0))	//Estop is triggered
					{
						isolatePack();
						ERRORS |= ESTOP_ERROR;
					}
					countSinceValidEStop[ESTOP_DE] = 0;
				}
				break;

			//Contactors enabled check
			case DE_STATUS:
				if(msg->data.data_u8[4] & BIT3)		//Contactors bit is set for motor
				{
					if(ERRORS == 0x00)
					{
						connectMotor();
						connectGround();
					}
				} else {
					disconnectMotor();
					if(!(OD_OUTPUTS & OD_ARRAY))	//If the array isn't on, disconnect ground
					{
						disconnectGround();
					}
				}
				break;

			case MC_BUS_MEASURE:	//Update the amount of input current allowable from regen every time the MC updates its current draw reading
				if(STATUS & STAT_CHARGE)
				{
					allowableInputCurrent = FAULT_CURRENT_CHARGE - analog_readings[ADC_PACK_CURRENT][AVERAGE_PERIOD] - msg->data.data_fp[1];
					if(allowableInputCurrent < (float) 0.98 * FAULT_CURRENT_CHARGE) allowableInputCurrent = (float) 0.98 * FAULT_CURRENT_CHARGE;
				} else {
					allowableInputCurrent = 0;
				}
		}
	}
}

/**
 * Handle input and processing for ADC values.
 *
 * First we measure all the ADCs we're watching with {@link measure_analogs},
 * then check {@link analog_readings} against the current setpoints for charging and discharging.
 *
 * @see measure_analogs()
 */
void processADCValues()
{
	measure_analogs();	//Take all ADC readings

	//Pack current fault detection
	if(analog_readings[ADC_PACK_CURRENT][AVERAGE_PERIOD] < FAULT_CURRENT_CHARGE)	//Charging at > 25A
	{
		ERRORS |= CHARGE_FAULT;
	}
	if(analog_readings[ADC_PACK_CURRENT][AVERAGE_PERIOD] > FAULT_CURRENT_DISCHARGE) //Discharging at > 75A
	{
		ERRORS |= DISCHARGE_FAULT;
	}

	//If charging is disabled, and current into the pack is more than 500ma, we need to fault
	if(  !(STATUS & STAT_CHARGE) && analog_readings[ADC_PACK_CURRENT][AVERAGE_PERIOD] < (float) -1.0 //Check if charging is disabled but we have current into pack
	  && timeSinceChargeDisable >= AVERAGE_PERIOD)		//Make sure enough time has elapsed for current readings to reaverage after disabling charging
	{
		ERRORS |= CHARGE_FAULT;
	}

	//Total pack voltage fault detection is taken care of by the LTC processor, so we don't need to measure it here

	//Aux pack will trigger a warning, but not a fault
	if(analog_readings[ADC_AUX_VOLTAGE][AVERAGE_PERIOD] < AUX_WARN_MIN)
	{
		WARNINGS |= AUX_FAULT;
	} else {
		WARNINGS &= ~AUX_FAULT;
	}
}

/**
 * Measure, read in, and check battery voltages for errors.
 *
 * 1. Begin a conversion and wait for it to complete
 * 2. Read data in to the cells[][] registers
 * 3. Check for over/under-voltage faults and warnings
 *
 * @see ltc6803Conv()
 * @see ltc6803ReadIn()
 */
void processVoltages()
{
	ltc6803Conv();																//Begin a voltage measurement
	ltc6803ReadIn();															//Read the voltages into the MSP430
	for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)							//Check each voltage value to be in range
	{
		for(int8_t cell = NUM_CELLS[bank]; cell >= CELL1; cell--)
		{
			if(cells[bank][cell].voltage[AVERAGE_PERIOD] > FAULT_VOLTAGE_MAX)
				ERRORS	 |= OV_FAULT;
			else if(cells[bank][cell].voltage[AVERAGE_PERIOD] < FAULT_VOLTAGE_MIN)
				ERRORS   |= UV_FAULT;
			else if(cells[bank][cell].voltage[AVERAGE_PERIOD] > WARN_VOLTAGE_MAX)
				WARNINGS |= OV_FAULT;
			else if(cells[bank][cell].voltage[AVERAGE_PERIOD] < WARN_VOLTAGE_MIN)
				WARNINGS |= UV_FAULT;
		}
	}
}

/**
 * Alternate between taking a temperature measurement and reading it in and calculating errors.
 *
 * Since the one-wire bus speed is ~32kbit/sec with {@link OW_SCTSPEED}, and we have to tx/rx 96bits/sensor
 * it takes several milliseconds per sensor to select the sensor then receive data. We have 26 sensors on
 * the bus, so it can pretty easily take 75-100ms to read in all the sensors at {@link OW_SCTSPEED}. The bus
 * isn't robust enough to support {@link OW_OVERDRIVE}, so we can't speed this up much.
 *
 * All this is a long way of saying that since the sensors need 186ms to complete a measurement, with a 5Hz
 * main loop we can't complete a temperature measurement and read it in during the same loop. As such, we
 * alternate between measuring on one call of the function, and then read in that measurement on the next.
 * Since we know that processTemperatures() only gets called once per main loop, and the main loop runs at
 * 200ms intervals, we can guarantee measurements have time to complete.
 *
 * @see ds18b20StartMeasure()
 * @see ds18b20ReadIn()
 */
void processTemperatures()
{
	static uint8_t measureOrRead = 0;
	if(measureOrRead == 0)
	{
		ds18b20StartMeasure();
		measureOrRead = 1;
	} else if(measureOrRead == 1)
	{
		ds18b20ReadIn();
		measureOrRead = 0;

		for(int8_t bank = NUM_BANKS; bank >= BANK1; bank--)							//Check each temperature value to be in range
		{
			for(int8_t cell = NUM_TEMPS[bank] - 1; cell >= CELL1; cell--)
			{
				if(analog_readings[ADC_PACK_CURRENT] >= 0) //Discharging
				{
					if(temps[bank][cell].temperature[AVERAGE_PERIOD] >= FAULT_TEMP_DISCHARGING || temps[bank][cell].temperature[AVERAGE_PERIOD] <= MIN_TEMP)
					{
						ERRORS |= TEMP_FAULT;	//Cell is too hot/cold
					} else if(temps[bank][cell].temperature[AVERAGE_PERIOD] >= WARN_TEMP_DISCHARGING || temps[bank][cell].temperature[AVERAGE_PERIOD] <= MIN_TEMP)
					{
						//Check if above warn current
						WARNINGS |= TEMP_FAULT;
					}
				} else if(analog_readings[ADC_PACK_CURRENT] < 0) //Charging
				{
					if(temps[bank][cell].temperature[AVERAGE_PERIOD] >= FAULT_TEMP_CHARGING || temps[bank][cell].temperature[AVERAGE_PERIOD] <= MIN_TEMP)
					{
						ERRORS |= TEMP_FAULT;	//Cell is too hot/cold
					}
					else if(temps[bank][cell].temperature[AVERAGE_PERIOD] >= WARN_TEMP_CHARGING || temps[bank][cell].temperature[AVERAGE_PERIOD] <= MIN_TEMP)
					{
						//Check if above warn current
						WARNINGS |= TEMP_FAULT;
					}
				}
			}
		}
	}
}

/**
 * Pushes all the messages described in the @ref CAN_SPEC into the {@link canq_tx CAN transmit queue}.
 */
void processCANMessages()
{
	//////////////////////
	//  CAN Processing
	//////////////////////
	//Status/Overall packet
	can_tx_push_ptr->address 		  = BMS_STATUS;
	can_tx_push_ptr->data.data_u16[0] = ERRORS;
	can_tx_push_ptr->data.data_u8[2]  = WARNINGS;
	can_tx_push_ptr->data.data_u8[3]  = STATUS;
	can_tx_push_ptr->data.data_fp[1]  = analog_readings[ADC_PACK_VOLTAGE][AVERAGE_PERIOD];
	can_push(CAN_TX);

	//Min and max voltage packet
	cell_t* minCell;
	cell_t* maxCell;
	minCell = getMinCell();
	maxCell = getMaxCell();
	can_tx_push_ptr->address 		 = BMS_VTG1;
	can_tx_push_ptr->data.data_fp[0] = maxCell->voltage[AVERAGE_PERIOD];
	can_tx_push_ptr->data.data_fp[1] = minCell->voltage[AVERAGE_PERIOD];
	can_push(CAN_TX);

	//Min & max locations + aux voltage packet
	can_tx_push_ptr->address 		 = BMS_VTG2;
	can_tx_push_ptr->data.data_u8[0]  = maxCell->bank;
	can_tx_push_ptr->data.data_u8[1]  = maxCell->cell;
	can_tx_push_ptr->data.data_u8[2]  = minCell->bank;
	can_tx_push_ptr->data.data_u8[3]  = minCell->cell;
	can_tx_push_ptr->data.data_fp[1] = analog_readings[ADC_AUX_VOLTAGE][AVERAGE_PERIOD];
	can_push(CAN_TX);

	//Min and max temperature value
	temp_t* minTemp;
	temp_t* maxTemp;
	minTemp = getMinTemp();
	maxTemp = getMaxTemp();
	can_tx_push_ptr->address = BMS_TEMP1;
	can_tx_push_ptr->data.data_fp[0] = maxTemp->temperature[AVERAGE_PERIOD];
	can_tx_push_ptr->data.data_fp[1] = minTemp->temperature[AVERAGE_PERIOD];
	can_push(CAN_TX);

	//Max temp address
	can_tx_push_ptr->address 		 = BMS_TEMP2;
	can_tx_push_ptr->data.data_u64   = maxTemp->addr;
	can_push(CAN_TX);

	//Min temp address
	can_tx_push_ptr->address 		 = BMS_TEMP3;
	can_tx_push_ptr->data.data_u64   = minTemp->addr;
	can_push(CAN_TX);

	//Current + min/max temp bank bitfield
	can_tx_push_ptr->address 		  = BMS_TEMP4;
	can_tx_push_ptr->data.data_fp[0]  = getPackTemp();
	//TODO: bank u8[4] and u8[5]
	can_tx_push_ptr->data.data_u8[4]  = countSinceValidEStop[ESTOP_UBCB];
	can_tx_push_ptr->data.data_u8[5]  = countSinceValidEStop[ESTOP_DE];
	can_tx_push_ptr->data.data_u8[6]  = minTemp->bank;
	can_tx_push_ptr->data.data_u8[7]  = maxTemp->bank;
	can_push(CAN_TX);

	can_tx_push_ptr->address		 = BMS_CURRENT;
	can_tx_push_ptr->data.data_fp[0] = analog_readings[ADC_PACK_CURRENT][AVERAGE_PERIOD];
	can_tx_push_ptr->data.data_fp[1] = allowableInputCurrent;
	can_push(CAN_TX);
	allowableInputCurrent = 0;

	//Pack Resistance - Unimplemented right now
	/*
	can_push_ptr->address		  = BMS_RESISTANCE;
	can_push_ptr->status		  = 8;
	can_push_ptr->data.data_16[0] = PACK RESISTACE
	can_push_ptr->data.data_16[1] = MIN CELL RESISTANCE
	can_push_ptr->data.data_16[2] = MAX CELL RESISTANCE
	can_push_ptr->data.data_u8[6] = MAX RESISTANCE BANK/CELL
	can_push_ptr->data.data_u8[7] = MIN RESISTANCE BANK/CELL
	can_push();
	*/
}

/**
 * Initialize all of the processors IO pins.
 *
 * 1. Set all pins to outputs with a value of 0. Will minimize power consumption.
 * 2. Move the ADC pins to high-impedance so we can actually measure them.
 * 3. Configure one-wire pins for all three banks.
 * 4. Disable the strong-pullup MOSFETS.
 * 5. Chip select pins high for all SPI devices.
 * 6. Enable the CAN interrupt pin.
 * 7. Give SPI pins to the peripheral library.
 * 8. Give ADC inputs to the peripheral library.
 */
void io_init()
{
	//This bit is a little weird, but it makes sense. To minimize power consumption and avoid floating pins
	//we set all IO pins to outputs with a value of 0. Then we change things away from that as necessary below
	P1DIR = 0xFF; P1OUT = 0x00;
	P2DIR = 0xFF; P2OUT = 0x00;
	P3DIR = 0xFF; P3OUT = 0x00;
	P4DIR = 0xFF; P4OUT = 0x00;
	P5DIR = 0xFF; P5OUT = 0x00;
	P6DIR = 0xFF; P6OUT = 0x00;
	P7DIR = 0xFF; P7OUT = 0x00;
	P8DIR = 0xFF; P8OUT = 0x00;

	//Set the ADC input pins to high-impedance so we don't sink to ground
	P6DIR &= ~(BIT0 + BIT1 + BIT2);

	//1-Wire Pins
	//These will flip back and forth from input to output constantly later, but for now just set them to high impedance inputs
	P4DIR &= ~BANK1_TEMP;
	P6DIR &= ~BANK2_TEMP;
	P1DIR &= ~BANK3_TEMP;

	//Set the strong pullup MOSFETS high
	P1OUT |= TEMP1_MOSFET + TEMP2_MOSFET + TEMP3_MOSFET;

	//Set the CS lines high
	P2OUT |= OPENDRAIN_CS;
	P3OUT |= LED_CS;
	P8OUT |= CAN_CS + LTC_CS;

	//CAN interrupts
	P1DIR &= ~CAN_INT;	//Clear this so we know it's an input
	P1IES |= CAN_INT;	//Falling edge trigger
	P1IFG &= ~CAN_INT;  //Interrupt flag cleared
	P1IE  |= CAN_INT;   //Enable pin interrupt

	//Give SPI to the peripheral library
	P3SEL |= MOSI + MISO + SCK;
	P3SEL |= MOSI_2 + MISO_2;
	P2SEL |= BIT7;

	//ADC Inputs to peripheral library
	P6SEL |= PACK_VOLTAGE + PACK_CURRENT + AUX_VOLTAGE;
}

/**
 * Initialize the system clock to the speed specified in {@link MCLK_FREQ}.
 */
void clock_init()
{
	//////////////////////////////////////
	// Below is FLL magic, just take this for what it is and don't question it
	//////////////////////////////////////
	UCSCTL3 &= ~(0x0070);					//Select XT1CLK (our onboard 32.768kHz crystal) as the FLL reference
	UCSCTL4 &= ~(0x0070);					//Select XT1CLK as ACLK

	uint16_t ratio, dco_div_bits;
	uint8_t mode = 0;						//DCOCLK or DCOCLKDIV?

	uint16_t sysfreq = MCLK_FREQ / 1000;	//System frequency in kHz

	ratio = MCLK_FREQ / 32768;				//Ratio of system freq -> FLL reference freq
	dco_div_bits = FLLD__2;					//Have at least a divider of 2

	if(sysfreq > 16000) {
		ratio >>= 1;
		mode = 1;							//Is sys freq > 16MHz? If so use DCOCLK rather than DCOCLKDIV
	} else sysfreq <<= 1;

	while(ratio > 512) {
		//Step up to the next div level
		dco_div_bits += FLLD0;
		ratio >>= 1;
	}

	__bis_SR_register(SCG0);				//Disable the FLL while we change the clocks

	UCSCTL0 = 0x0000;						//Set the DCO to the lowest tap setting

	UCSCTL2 &= ~(0x3FF);					//Clear the 8 LSBs in the register to clear the FLLN bits
	UCSCTL2 = dco_div_bits | (ratio - 1);	//Datasheet voodoo

	//sysfreq is in kHz
	if (sysfreq <= 630)						//Set the proper DCORSEL value
		UCSCTL1= DCORSEL_0;
    else if (sysfreq <  1250)
    	UCSCTL1= DCORSEL_1;
	else if (sysfreq <  2500)
		UCSCTL1= DCORSEL_2;
	else if (sysfreq <  5000)
		UCSCTL1= DCORSEL_3;
	else if (sysfreq <  10000)
		UCSCTL1= DCORSEL_4;
	else if (sysfreq <  20000)
		UCSCTL1= DCORSEL_5;
	else if (sysfreq <  40000)
		UCSCTL1= DCORSEL_6;
	else
		UCSCTL1= DCORSEL_7;

	__bic_SR_register(SCG0);				//Re-enable the FLL

	/* In theory this should be required, but it was hanging because these were apparently never being cleared?
	 * I dunno. It works without them. If something breaks with the clocks and hanging at init, it's probably here
	 *
	 * while(SFRIFG1 & OFIFG) {
		UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
		SFRIFG1 &= ~OFIFG;
	}*/

	if(mode == 1) { 						//select DCOCLK
		UCSCTL4 &= ~(0x0077);
		UCSCTL4 |= 0x0033;
	} else {								//Select DCOCLKDIV
		UCSCTL4 &= ~(0x0077);
		UCSCTL4 |= 0x0044;
	}

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 20 MHz / 32,768 Hz = 625000 MCLK cycles for DCO to settle
	__delay_cycles(625000);
}

/**
 * Initialize Timer A0 to capture-compare mode, enable the interrupt, with a period of 200ms
 */
void timer_init()
{
	TA0CTL 	 |= TASSEL_1 + MC_1;  			// ACLK, contmode, clear TAR
	TA0CCTL0 |= CCIE;
	TA0CCR0   = 6554;						// Count Limit 1/5th of ACLK for 200ms
}
