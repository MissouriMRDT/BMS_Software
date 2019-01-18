/* Programmer: Drue Satterfield
 * Date of creation: October 16, 2017
 * Microcontroller used: MSP432P401R
 * Hardware components used by this file: System clocks
 *
 * Description: This library is used to control the power mode of the device. It has in general 3 power states it can cycle through;
 * high power mode, which consumes more power but runs faster, low power mode, which consumes less power but runs slower, and low frequency
 * mode which is the slowest and least power consuming of all. All of these states affect the max value of the main system clocks.
 *
 * As well, the chip can decide whether it wants to use an internal LDO to power its system, or an internal DC/DC converter.
 * The former can be used anywhere but consumes a bit more power, while the latter is more efficient but requires the hardware engineer to
 * wire up some special external components to use it (see the device's datasheet).
 *
 * Power consumption on average:
 *
 *        High voltage        Low Voltage
 * LDO        7.7mA              4.26mA
 * DC/DC      4.7mA              2.06mA
 * Low Freq    .108mA             .098mA
 *
 */


#ifndef ROVEBOARD_MSP432P401R_API_POWER_POWER_MSP432P401R_H_
#define ROVEBOARD_MSP432P401R_API_POWER_POWER_MSP432P401R_H_

//tells the system to start using its internal DC/DC regulator as a source. Note that this will only work if the chip has had the proper
//components set up externally to allow this mode; consult the hardware engineers who are making your pcb to see if this mode is supported.
//returns: true if successful, false otherwise.
bool useSwitchingPowerSource();

//tells the system to start using its internal LDO power source as a...power source.
//returns: true if successful, false otherwise.
//note: This is the default option on startup.
bool useLDOPowerSource();

//tells the system to switch to low frequency mode. This will force the cpu clock and the periph clock to 128Khz frequency.
//note that this will affect all running peripherals; you should re-init any you're using to adjust for the new clock frequency.
//Technically you can choose whether to put it into high or low voltage mode at the same time. No real functional difference, you usually want to
//select false to consume less power.
//warning: This frequency is low enough to where you'll have to start worrying about whether or not your periphs can even operate as you
//desire; for most of them, if you tell them to operate at some period above the main or periph clock frequency they won't work. For example,
//the UART's won't be able to have a baud rate above 128000.
void forceLowFrequencyMode(bool putInHighVoltageMode);

//tells the system to switch to low voltage mode. This will force the cpu clock to 24Mhz and the periph clock to 12Mhz frequency.
//note that this will affect all running peripherals; you should re-init any you're using to adjust for the new clock frequency.
void forceLowVoltageMode();

//tells the system to switch to high voltage mode. This will force the cpu clock to 48Mhz and the periph clock to 24Mhz frequency.
//note that this will affect all running peripherals; you should re-init any you're using to adjust for the new clock frequency.
//note: this is the default optino on startup.
void forceHighVoltageMode();


#endif /* ROVEBOARD_MSP432P401R_API_POWER_POWER_MSP432P401R_H_ */
