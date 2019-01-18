/*
 * RoveADC_TivaTM4C1294NCPDT.cpp
 *
 *  Created on: Oct 27, 2017
 *      Author: drue
 */

#include "../Clocking/Clocking_TivaTM4C1294NCPDT.h"
#include "../RovePinMap_TivaTM4C1294NCPDT.h"
#include "supportingUtilities/Debug.h"
#include "supportingUtilities/RoveUtilities.h"
#include "../tivaware/inc/hw_memmap.h"
#include "../tivaware/inc/hw_types.h"
#include "../tivaware/inc/hw_adc.h"
#include "../tivaware/inc/hw_gpio.h"
#include "../tivaware/inc/hw_ints.h"
#include "../tivaware/driverlib/adc.h"
#include "../tivaware/driverlib/interrupt.h"
#include "../tivaware/driverlib/sysctl.h"
#include "../tivaware/driverlib/gpio.h"
#include "../tivaware/driverlib/pin_map.h"
#include "RoveADC_TivaTM4C1294NCPDT.h"

//If you try to make a conversion with the FIFO full, you'll get an overflow error. If you call getConvResult with nothing in the FIFO, you get an underflow error.
//These errors are both completely avoidable by checking the EMPTY and FULL bits before getConvResult and startConv respectively.
//Index into this with channel_number * 3 for pin and port info to set up. SYSCYTL_PERIPH_THING is just THING_BASE + 0xB0000000, but we have plenty of memory and asking the user to remember this conversion is stupid. If we need the space, we'll just create a macro to do it.
//Offset 0 is the GPIO port base, offset 1 is the SYSCTL'd version, and offset 2 is the pin number.

const uint8_t NotAChan = 25;

const uint32_t pin_to_adc_chan[] =
{
  NotAChan,          // dummy
  NotAChan,          // 01 - 3.3v       X8_01
  9,            // 02 - PE_4       X8_03
  NotAChan,            // 03 - PC_4       X8_05
  NotAChan,            // 04 - PC_5       X8_07
  NotAChan,            // 05 - PC_6       X8_09
  8,            // 06 - PE_5       X8_11
  12,            // 07 - PD_3       X8_13
  NotAChan,            // 08 - PC_7       X8_15
  NotAChan,            // 09 - PB_2       X8_17
  NotAChan,            // 10 - PB_3       X8_19
  NotAChan,            // 11 - PP_2       X9_20
  NotAChan,            // 12 - PN_3       X9_18
  NotAChan,            // 13 - PN_2       X9_16
  15,            // 14 - PD_0       X9_14
  14,            // 15 - PD_1       X9_12
  NotAChan,          // 16 - RST        X9_10
  NotAChan,            // 17 - PH_3       X9_08
  NotAChan,            // 18 - PH_2       X9_06
  NotAChan,            // 19 - PM_3       X9_04
  NotAChan,          // 20 - GND        X9_02
  NotAChan,          // 21 - 5v         X8_02
  NotAChan,          // 22 - GND        X8_04
  3,            // 23 - PE_0       X8_06
  2,            // 24 - PE_1       X8_08
  1,            // 25 - PE_2       X8_10
  0,            // 26 - PE_3       X8_12
  4,            // 27 - PD_7       X8_14
  NotAChan,            // 28 - PA_6       X8_16
  NotAChan,            // 29 - PM_4       X8_18
  NotAChan,            // 30 - PM_5       X8_20
  NotAChan,            // 31 - PL_3       X9_19
  NotAChan,            // 32 - PL_2       X9_17
  NotAChan,            // 33 - PL_1       X9_15
  NotAChan,            // 34 - PL_0       X9_13
  NotAChan,            // 35 - PL_5       X9_11
  NotAChan,            // 36 - PL_4       X9_09
  NotAChan,            // 37 - PG_0       X9_07
  NotAChan,            // 38 - PF_3       X9_05
  NotAChan,            // 39 - PF_2       X9_03
  NotAChan,            // 40 - PF_1       X9_01
  NotAChan,          // 41 - 3.3v       X6_01
  13,            // 42 - PD_2       X6_03
  NotAChan,            // 43 - PP_0       X6_05
  NotAChan,            // 44 - PP_1       X6_07
  7,            // 45 - PD_4       X6_09
  6,            // 46 - PD_5       X6_11
  NotAChan,            // 47 - PQ_0       X6_13
  NotAChan,            // 48 - PP_4       X6_15
  NotAChan,            // 49 - PN_5       X6_17
  NotAChan,            // 50 - PN_4       X6_19
  NotAChan,            // 51 - PM_6       X7_20
  NotAChan,            // 52 - PQ_1       X7_18
  NotAChan,            // 53 - PP_3       X7_16
  NotAChan,            // 54 - PQ_3       X7_14
  NotAChan,            // 55 - PQ_2       X7_12
  NotAChan,          // 56 - RESET      X7_10
  NotAChan,            // 57 - PA_7       X7_08
  NotAChan,            // 58 - PP_5       X7_06
  NotAChan,            // 59 - PM_7       X7_04
  NotAChan,          // 6Z - GND        X7_02
  NotAChan,          // 61 - 5v         X6_02
  NotAChan,          // 62 - GND        X6_04
  10,            // 63 - PB_4       X6_06
  11,            // 64 - PB_5       X6_08
  16,            // 65 - PK_0       X6_10
  17,            // 66 - PK_1       X6_12
  18,            // 67 - PK_2       X6_14
  19,            // 68 - PK_3       X6_16
  NotAChan,            // 69 - PA_4       X6_18
  NotAChan,            // 70 - PA_5       X6_20
  NotAChan,            // 71 - PK_7       X7_19
  NotAChan,            // 72 - PK_6       X7_17
  NotAChan,            // 73 - PH_1       X7_15
  NotAChan,            // 74 - PH_0       X7_13
  NotAChan,            // 75 - PM_2       X7_11
  NotAChan,            // 76 - PM_1       X7_09
  NotAChan,            // 77 - PM_0       X7_07
  NotAChan,            // 78 - PK_5       X7_05
  NotAChan,            // 79 - PK_4       X7_03
  NotAChan,            // 80 - PG_1       X7_01
  NotAChan,            // 81 - PN_1       LED1
  NotAChan,            // 82 - PN_0       LED2
  NotAChan,            // 83 - PF_4       LED3
  NotAChan,            // 84 - PF_0       LED4
  NotAChan,            // 85 - PJ_0       USR_SW1
  NotAChan,            // 86 - PJ_1       USR_SW2
  5,            // 87 - PD_6       AIN5
  NotAChan,            // 88 - PA_0       JP4
  NotAChan,            // 89 - PA_1       JP5
  NotAChan,            // 90 - PA_2       X11_06
  NotAChan,            // 91 - PA_3       X11_08
  NotAChan,            // 92 - PL_6       unrouted
  NotAChan,            // 93 - PL_7       unrouted
  NotAChan,            // 94 - PB_0       X11_58
  NotAChan,            // 95 - PB_1       unrouted
};

const uint32_t adc_chan_sysctl[20] = {
  SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOE,
  SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD,
  SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOE,
  SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOB,
  SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOD,
  SYSCTL_PERIPH_GPIOK, SYSCTL_PERIPH_GPIOK, SYSCTL_PERIPH_GPIOK, SYSCTL_PERIPH_GPIOK
};

const uint8_t adc_chan_pin[20] = {
  GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_0,
  GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4,
  GPIO_PIN_5, GPIO_PIN_4,
  GPIO_PIN_4, GPIO_PIN_5,
  GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_0,
  GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
};

const uint32_t adc_chan_base[20] = {
  GPIO_PORTE_AHB_BASE, GPIO_PORTE_AHB_BASE, GPIO_PORTE_AHB_BASE, GPIO_PORTE_AHB_BASE,
  GPIO_PORTD_AHB_BASE, GPIO_PORTD_AHB_BASE, GPIO_PORTD_AHB_BASE, GPIO_PORTD_AHB_BASE,
  GPIO_PORTE_AHB_BASE, GPIO_PORTE_AHB_BASE,
  GPIO_PORTD_AHB_BASE, GPIO_PORTD_AHB_BASE, GPIO_PORTD_AHB_BASE, GPIO_PORTD_AHB_BASE,
  GPIO_PORTB_AHB_BASE, GPIO_PORTB_AHB_BASE,
  GPIO_PORTK_BASE, GPIO_PORTK_BASE, GPIO_PORTK_BASE, GPIO_PORTK_BASE,
  };
const uint32_t adc_chans[20] = { //This may look odd, but everything above 15 has 0xF0 added and this is less computation.
  ADC_CTL_CH0, ADC_CTL_CH1, ADC_CTL_CH2, ADC_CTL_CH3,
  ADC_CTL_CH4, ADC_CTL_CH5, ADC_CTL_CH6, ADC_CTL_CH7,
  ADC_CTL_CH8, ADC_CTL_CH9, ADC_CTL_CH10, ADC_CTL_CH11,
  ADC_CTL_CH12, ADC_CTL_CH13, ADC_CTL_CH14, ADC_CTL_CH15,
  ADC_CTL_CH16, ADC_CTL_CH17, ADC_CTL_CH18, ADC_CTL_CH19
};

//const uint32_t adc0_ints[4] = {INT_ADC0SS0, INT_ADC0SS1, INT_ADC0SS2, INT_ADC0SS3};

//const uint32_t adc1_ints[4] = {INT_ADC1SS0, INT_ADC1SS1, INT_ADC1SS2, INT_ADC1SS3};

//const uint32_t adc_int_flags[4] = {ADC_INT_SS0, ADC_INT_SS1, ADC_INT_SS2, ADC_INT_SS3};

const uint32_t adc_ssfstat[4] = {ADC_O_SSFSTAT0, ADC_O_SSFSTAT1, ADC_O_SSFSTAT2, ADC_O_SSFSTAT3};

//const uint32_t adc_dma_int_flags[4] = {ADC_INT_DMA_SS0, ADC_INT_DMA_SS1, ADC_INT_DMA_SS2, ADC_INT_DMA_SS3};

//const uint32_t adc_dcon_int_flags[4] = {ADC_INT_DCON_SS0, ADC_INT_DCON_SS1, ADC_INT_DCON_SS2, ADC_INT_DCON_SS3};

#define STEP0 0x01
#define STEP1 0x02
#define STEP2 0x04
#define STEP3 0x08
#define STEP4 0x10
#define STEP5 0x20
#define STEP6 0x40
#define STEP7 0x80

#define PLLVCO ADC_CLOCK_SRC_PLL
#define PIOSC ADC_CLOCK_SRC_PIOSC
#define ALTCLK ADC_CLOCK_SRC_ALTCLK
#define MOSC ADC_CLOCK_SRC_MOSC

#define DEFAULT_ADC_CLK_SPEED 32000000
#define DEFAULT_ADC_SAMPLE_CYCLES 16 //at 16 nsh, we get about 1 million samples per second and allow up to about 10Kohm of external resistance.
#define SEQ_DELAY_FACTOR_MICROS 500 //how many microseconds we want to pass where if the user asks, a sequencer will just return its last reading
#define MAX_CHANS 20

#define SEQ0_LEN  8
#define SEQ1_LEN 4
#define SEQ2_LEN 4
#define SEQ3_LEN 1

typedef enum adc_instance {ADC0, ADC1} adc_instance;

typedef struct adc_conf {
  uint32_t adc;
  uint32_t vref_ext; //Voltage reference select; 0 for VDDA, 1 for VREFA+.
  uint32_t adc_clksrc;
  uint32_t vco_div;
  bool initialized;

  adc_conf()
  {
    adc = 0; //consts that fill this are never 0
  }
} adc_conf;

//Check spmu298 for valid sequence trigger sources

typedef struct seq_conf {
  uint32_t adc;
  uint32_t nsh[8]; //Sample and hold time in clock cycles.
  uint32_t sequencer;
  uint32_t trigger;
  uint32_t priority;
  uint32_t len;
  uint32_t chanseq[8]; //Sequence of channels to do conversions on when sequencer is triggered. Array is length of sequencer 0. Must set any unused sequence steps to 0xFF.
  uint32_t step_ints[8];
  uint32_t results[8];
  uint64_t lastTimestamp;

  seq_conf()
  {
    lastTimestamp = 0;
    adc = 0; //consts that fill this are never 0
  }
} seq_conf;

static seq_conf Adc0S0, Adc0S1, Adc0S2, Adc0S3, Adc1S0, Adc1S1, Adc1S2, Adc1S3;
static adc_conf Adc0, Adc1;
static uint32_t seqDelayFactor_us = SEQ_DELAY_FACTOR_MICROS;

//Valid inputs:
//config must be an uninitialized adc_conf. The function will fill out this structure for you.
//adc must be ADC0 or ADC1 as per the adc_instance enum. The definition of this may be different on other platforms with a different number of ADC modules.
//vco_div is the factor by which to divide the ADC's clock if the PLL is used as an input. THIS MUST PRODUCE A CLOCK OF 32MHz OR LESS, or the function will fail.
//adc_clksrc is one of PLLVCO, ALTCLK, PIOSC, or MOSC, as defined above. See the tm4c1294ncpdt user's guide for more information.
static bool adcInit(adc_conf& config, adc_instance adc, bool vref_ext, uint32_t vco_div, uint32_t adc_clksrc);

//Valid inputs:
//config must be an uninitialized seq_conf. The function will fill out this structure for you.
//adc must be ADC0 or ADC1 as per the adc_instance enum, and must correspond to an ADC you configured before with adc_init.
//len is the number of valid items in the array (i.e. how many channels total are in your sequence?). This must be 8 or less. The function will error if you specify a length no currently unused sequencer can handle.
//chans must be a pointer to an array of up to 8 channel numbers from 0 to 19. See the tm4c1294ncpdt user's guide for information on what channels map to what pin.
bool adcSeqInit(seq_conf& config, adc_conf& adc_info, uint8_t sequencerNum, uint32_t len, uint8_t* channels, bool* interrupts, uint32_t* nsh, uint32_t trigger, uint32_t priority);

static bool adcStartConv(seq_conf& config);

static bool adcGetConvResult(seq_conf& config);

static uint8_t getVcoDiv(const uint32_t targetSpeed);

static void addAdcChannel(uint8_t channel, seq_conf *seq, uint8_t chanseqFifoIndex, bool endOfSequence);

//*****************************************************************************************//
//*****************************************************************************************//
//        adcInit - Initialize a chosen ADC  and fill out a struct with info about it      //
//*****************************************************************************************//
//*****************************************************************************************//

bool adcInit(adc_conf& config, adc_instance adc, bool vref_ext, uint32_t vco_div, uint32_t adc_clksrc) { //make adc an enum; change sequencer to just length of FIFO
  uint32_t sysctl_adc;
  config.adc = adc ? ADC1_BASE : ADC0_BASE;
  sysctl_adc = adc ? SYSCTL_PERIPH_ADC1 : SYSCTL_PERIPH_ADC0;
  switch(adc_clksrc) {
    case PLLVCO:
      if((getCpuClockFreq() / vco_div) > 32e6) {
        return false;
      }
      config.adc_clksrc = ADC_CLOCK_SRC_PLL;
      config.vco_div = vco_div;
      break;
    case PIOSC:
      config.adc_clksrc = ADC_CLOCK_SRC_PIOSC; //Fail if vco_div != 1 instead of silently ignoring it
      if(vco_div != 1) {
        return false;
      }
      else {
      config.vco_div = 1;
      }
      break;
    case MOSC:
      config.adc_clksrc = ADC_CLOCK_SRC_MOSC;
      if(vco_div != 1) {
        return false;
      }
      else {
      config.vco_div = 1;
      }
      break;
  }

  config.vref_ext = vref_ext ? ADC_REF_EXT_3V : ADC_REF_INT;

  SysCtlPeripheralEnable(sysctl_adc);
  while(!SysCtlPeripheralReady(sysctl_adc));  //Accessing anything ADC-related before the peripheral is enabled is a bus fault.

  ADCClockConfigSet(config.adc, (config.adc_clksrc | ADC_CLOCK_RATE_FULL), config.vco_div);

  //Change to external voltage reference if requested.
  ADCReferenceSet(ADC0_BASE, config.vref_ext);
  return true;
}


//*****************************************************************************************//
//*****************************************************************************************//
// adcSeqInit - Initialize a chosen ADC sequencer and fill out a struct with info about it //
//*****************************************************************************************//
//*****************************************************************************************//


//The available clock sources are PIOSC, MOSC, and PLLVCO. The scaling factor only matters for PLLVCO and MUST scale it down to
//32MHz or lower for correct operation. MOSC and PIOSC cannot be scaled. PIOSC 16MHz gives 1 Msps, divided PLLVCO at 32MHz gives 2Msps.
//However, with the roveboard default of 120MHz PLL, it is impossible to divide to anything valid above 30MHz, as only integer divisors
//from 1 to 64 are supported (6-bit bitfield).



bool adcSeqInit(seq_conf& config, adc_conf& adc_info, uint8_t sequencerNum, uint32_t len, uint8_t* channels, bool* interrupts, uint32_t* nsh, uint32_t trigger, uint32_t priority) { //TODO: Allow specification of dcon/dma interrupts too.
  uint32_t i;
  config.adc = adc_info.adc;
  switch(len) {
    case 1:
      //Try sequencers starting with the smallest. If they're in use or don't fit the sequence length, go on.
      if(!(HWREG(config.adc + ADC_O_ACTSS) & ADC_ACTSS_ASEN3)) {
        config.sequencer = 3;
        break;
      }
    case 2:
    case 3:
    case 4:
      if(!(HWREG(config.adc + ADC_O_ACTSS) & ADC_ACTSS_ASEN2) && sequencerNum == 2) {
        config.sequencer = 2;
        break;
      }
      else if(!(HWREG(config.adc + ADC_O_ACTSS) & ADC_ACTSS_ASEN1) && sequencerNum == 1) {
        config.sequencer = 1;
        break;
      }
    case 5:
    case 6:
    case 7:
    case 8:
      if(!(HWREG(config.adc + ADC_O_ACTSS) & ADC_ACTSS_ASEN0)) {
        config.sequencer = 0;
        break;
      }
    default:
      return false; //No suitable sequencers available.

  }

  for(i = 0; i < len; i++)
  {
    if(channels[i] < MAX_CHANS)
    {
      config.chanseq[i]=channels[i];
    }
    else
    {
      return false;
    }
    if(nsh[i] & (nsh[i] - 1))
    {
      return false;
    }
    else
    {
      config.nsh[i] = nsh[i];
    }
    if(interrupts[i])
    {
      config.step_ints[i] = ADC_CTL_IE;
    }
    else
    {
      config.step_ints[i] = 0;
    }
  }

  if((trigger <= ADC_TRIGGER_PWM3) || ((trigger >= ADC_TRIGGER_NEVER) && (trigger <= ADC_TRIGGER_ALWAYS))) {
    config.trigger = trigger;
  }
  else {
    return false;
  }

  if(priority < 8) { //priority only goes up to 7 on tiva
    config.priority = priority;
  }
  else {
    return false;
  }
  //Input checks:
  // -sequencer and sample priority must be 0, 1, 2, or 3
  // -Sample and hold window must be a power of 2 clock cycles long.
  // -Must not configure more samples than the depth of the chosen FIFO
  //Must specify a valid trigger source. See driverlib/adc.h and spmu298.
  // -nsh must be a power of 2.

  //Ensure sequencer is disabled.
  //Configure sequencer, trigger source and priority



  //Enable clock to GPIO modules used. ADC channel numbers map to GPIO ports as follows:
  // -PE: 0-3, 8-9
  // -PD: 4-7, 12-15
  // -PB: 10-11
  // -PK: 16-19
  //
  for(i = 0; i < len; i++) {
    if ((config.chanseq[i] < MAX_CHANS) && (config.chanseq[i] != 4) && (!SysCtlPeripheralReady(adc_chan_sysctl[config.chanseq[i]]))) {
      SysCtlPeripheralEnable(adc_chan_sysctl[config.chanseq[i]]);
    }
    else if (config.chanseq[i] == 4) { //Channel 4 is PD7, which is locked for commit by default and must be freed.
      if(!((GPIO_PORTD_BASE + GPIO_O_CR) & GPIO_PIN_7)) {
        HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
      }
      if(!SysCtlPeripheralReady(adc_chan_sysctl[4])) {
        SysCtlPeripheralEnable(adc_chan_sysctl[config.chanseq[i]]);
      }
    }
  }


  //Set AFSEL for selected channel pins
  //Disable the analog isolation circuit for the selected channel pins
  for(i = 0; i < len; i++) {
    GPIOPinTypeADC(adc_chan_base[config.chanseq[i]], adc_chan_pin[config.chanseq[i]]);
  }

  ADCSequenceDisable(config.adc, config.sequencer);

  ADCSequenceConfigure(config.adc, config.sequencer, config.trigger, config.priority);

  config.len = len;
  for(i=0; i < config.len - 1; i++) {
    if(config.chanseq[i] == 0xFF) {
      return false;
    }
    ADCSequenceStepConfigure(config.adc, config.sequencer, i, (adc_chans[config.chanseq[i]] | (config.nsh[i] << MAX_CHANS) | (config.step_ints[i])));
  }
  if(config.chanseq[config.len - 1] == 0xFF) {
    return false;
  }
  else {
    ADCSequenceStepConfigure(config.adc, config.sequencer, config.len - 1,
        (adc_chans[config.chanseq[config.len - 1]] | (config.nsh[config.len - 1] << MAX_CHANS) | (config.step_ints[len - 1]) | ADC_CTL_END));
  }

  //Set the interrupt mask bits for the chosen sequence step interrupts.
  /*ADCIntEnableEx(config.adc, adc_int_flags[config.sequencer]); //we don't want an actual interrupt to be generated, we just currently
  if(config.adc == ADC0_BASE) {                                  //use the flag to detect when a conv is complete
    IntEnable(adc0_ints[config.sequencer]);
  }
  else {
    IntEnable(adc1_ints[config.sequencer]);
  }*/
  //Enable the sample sequencer.
  ADCSequenceEnable(config.adc, config.sequencer);
  return true;
}

RoveAdc_Handle roveAdc_init(uint16_t module, uint8_t pin)
{
  uint8_t channel = pin_to_adc_chan[pin];
  uint8_t sequencer;
  uint8_t vcoDiv;
  uint8_t seqLength;
  bool endOfSequence= false;
  uint8_t moduleOffset;
  seq_conf* seqToConf;
  adc_conf *adcToConf;

  if(module > 33)
  {
    debugFault("roveADC_init: module is nonsense");
  }

  else if(channel == NotAChan)
  {
    debugFault("roveADC_init: pin is nonsense");
  }

  //init adc module, if necessary
  if(module < Adc1_Seq0_0)
  {
    if(Adc0.adc == 0) //adc module is just being started
    {
     vcoDiv = getVcoDiv(DEFAULT_ADC_CLK_SPEED);
     adcInit(Adc0, ADC0, false, vcoDiv, PLLVCO);
    }
    else {} //nothing special if the adc module has already been started
  }
  else
  {
    if(Adc1.adc == 0) //adc module is just being started
    {
     vcoDiv = getVcoDiv(DEFAULT_ADC_CLK_SPEED);
     adcInit(Adc1, ADC1, false, vcoDiv, PLLVCO);
    }
    else {} //nothing special if the adc module has already been started
  }

  //get adc sequencer information from module
  if(module <= Adc0_Seq0_7)
  {
    adcToConf = &Adc0;
    seqToConf = &Adc0S0;
    moduleOffset = 0;
    seqLength = SEQ0_LEN;
    sequencer = 0;
    if(module == Adc0_Seq0_7)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc0_Seq1_3)
  {
    adcToConf = &Adc0;
    seqToConf = &Adc0S1;
    moduleOffset = Adc0_Seq1_0;
    seqLength = SEQ1_LEN;
    sequencer = 1;
    if(module == Adc0_Seq1_3)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc0_Seq2_3)
  {
    adcToConf = &Adc0;
    seqToConf = &Adc0S2;
    moduleOffset = Adc0_Seq2_0;
    seqLength = SEQ2_LEN;
    sequencer = 2;
    if(module == Adc0_Seq2_3)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc0_Seq3_0)
  {
    adcToConf = &Adc0;
    seqToConf = &Adc0S3;
    moduleOffset = Adc0_Seq3_0;
    seqLength = SEQ3_LEN;
    sequencer = 3;
    if(module == Adc0_Seq3_0)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc1_Seq0_7)
  {
    adcToConf = &Adc1;
    seqToConf = &Adc1S0;
    moduleOffset = Adc1_Seq0_0;
    seqLength = SEQ0_LEN;
    sequencer = 0;
    if(module == Adc1_Seq0_7)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc1_Seq1_3)
  {
    adcToConf = &Adc1;
    seqToConf = &Adc1S1;
    moduleOffset = Adc1_Seq1_0;
    seqLength = SEQ1_LEN;
    sequencer = 1;
    if(module == Adc1_Seq1_3)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc1_Seq2_3)
  {
    adcToConf = &Adc1;
    seqToConf = &Adc1S2;
    moduleOffset = Adc1_Seq2_0;
    seqLength = SEQ2_LEN;
    sequencer = 2;
    if(module == Adc1_Seq2_3)
    {
      endOfSequence = true;
    }
  }
  else if(module <= Adc1_Seq3_0)
  {
    adcToConf = &Adc1;
    seqToConf = &Adc1S3;
    moduleOffset = Adc1_Seq3_0;
    seqLength = SEQ3_LEN;
    sequencer = 3;
    if(module == Adc1_Seq3_0)
    {
      endOfSequence = true;
    }
  }

  //init adc sequencer or just set a channel if already initted
  if(seqToConf->adc == 0) //sequence just being started
  {
    bool ints[] = {true, true, true, true, true, true, true, true};
    uint32_t sampleHoldTimes[] = {DEFAULT_ADC_SAMPLE_CYCLES, DEFAULT_ADC_SAMPLE_CYCLES, DEFAULT_ADC_SAMPLE_CYCLES, DEFAULT_ADC_SAMPLE_CYCLES,
       DEFAULT_ADC_SAMPLE_CYCLES, DEFAULT_ADC_SAMPLE_CYCLES, DEFAULT_ADC_SAMPLE_CYCLES, DEFAULT_ADC_SAMPLE_CYCLES};
    uint8_t channels[] = {0, 0, 0, 0, 0, 0, 0, 0};
    channels[module - moduleOffset] = channel;

    adcSeqInit(*seqToConf, *adcToConf, sequencer, seqLength, channels, ints, sampleHoldTimes, ADC_TRIGGER_PROCESSOR, 0);
  }
  else //sequencer already started, add another channel
  {
    addAdcChannel(channel, seqToConf, module - moduleOffset, endOfSequence);
  }

  RoveAdc_Handle handle;
  handle.index = module;
  handle.initialized = true;

  return handle;
}

static uint8_t getVcoDiv(const uint32_t targetSpeed)
{
  uint32_t vcoRate; //either gonna be 320Mhz or 480Mhz
  SysCtlVCOGet(SYSCTL_XTAL_25MHZ, &vcoRate);

  int i;
  for(i = 1; i <= 64; i++) //divider can go up to 64 on this device
  {
    if(targetSpeed >= (vcoRate / i)) //round lower as device can't go above 32Mhz
    {
      return i;
    }
  }

  return 0;
}

static void addAdcChannel(uint8_t channel, seq_conf *seq, uint8_t chanseqFifoIndex, bool endOfSequence)
{
  #ifndef ROVEDEBUG_NO_DEBUG
  if(chanseqFifoIndex > 8)
  {
    debugFault("Drue you dumbass you didn't constrain inputs properly");
  }
  #endif

  seq->chanseq[chanseqFifoIndex] = channel;

  if(seq->chanseq[chanseqFifoIndex] == 4)  //Channel 4 is PD7, which is locked for commit by default and must be freed.
  {
    if(!((GPIO_PORTD_BASE + GPIO_O_CR) & GPIO_PIN_7))
    {
      HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
      HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
    }
  }

  SysCtlPeripheralEnable(adc_chan_sysctl[seq->chanseq[chanseqFifoIndex]]);

  GPIOPinTypeADC(adc_chan_base[seq->chanseq[chanseqFifoIndex]], adc_chan_pin[seq->chanseq[chanseqFifoIndex]]);

  ADCSequenceDisable(seq->adc, seq->sequencer);

  ADCSequenceConfigure(seq->adc, seq->sequencer, seq->trigger, seq->priority);

  uint8_t seqEnd;
  if(endOfSequence)
  {
    seqEnd = ADC_CTL_END;
  }
  else
  {
    seqEnd = 0;
  }

  ADCSequenceStepConfigure(seq->adc, seq->sequencer, chanseqFifoIndex,
      (adc_chans[seq->chanseq[chanseqFifoIndex]] | (seq->nsh[chanseqFifoIndex] << MAX_CHANS) | (seq->step_ints[chanseqFifoIndex] | seqEnd)));

  ADCSequenceEnable(seq->adc, seq->sequencer);
}

bool adcStartConv(seq_conf& config)
{
  //Check if the FIFO is full; clear it out if so
  if(HWREG(config.adc + adc_ssfstat[config.sequencer]) & ADC_SSFSTAT0_FULL) {
    ADCSequenceDataGet(config.adc, config.sequencer, config.results);
  }

  ADCIntClear(config.adc, config.sequencer);
  ADCProcessorTrigger(config.adc, config.sequencer);

  SysCtlDelay(100); //processor never starts conversion without it for some reason.
  return true;
}

bool adcGetConvResult(seq_conf& config)
{
  //Check if the sequencer has completed its transaction yet.
  //If so,
  //Get the sequencer's data and put it into the specified buffer.
  //ADCSequnceDataGet returns number of samples extracted from the FIFO. If this doesn't match the configured length of the sequence, you got the data before it was ready, or messed up something else.
  if(ADCIntStatus(config.adc, config.sequencer, false) > 0)
  {
    ADCIntClear(config.adc, config.sequencer);
    if(ADCSequenceDataGet(config.adc, config.sequencer, config.results) != config.len)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

RoveAdc_Error roveAdc_getConvResults(RoveAdc_Handle handle, uint32_t *retBuff)
{
  if(handle.initialized == false)
  {
    debugFault("roveAdc_getConvResults: Handle not initialized");
  }
  uint8_t moduleOffset;
  seq_conf *seq;

  uint8_t module = handle.index;

  //get adc sequencer information from module
  if(module <= Adc0_Seq0_7)
  {
    seq = &Adc0S0;
    moduleOffset = 0;
  }
  else if(module <= Adc0_Seq1_3)
  {
    seq = &Adc0S1;
    moduleOffset = Adc0_Seq1_0;
  }
  else if(module <= Adc0_Seq2_3)
  {
    seq = &Adc0S2;
    moduleOffset = Adc0_Seq2_0;
  }
  else if(module <= Adc0_Seq3_0)
  {
    seq = &Adc0S3;
    moduleOffset = Adc0_Seq3_0;
  }
  else if(module <= Adc1_Seq0_7)
  {
    seq = &Adc1S0;
    moduleOffset = Adc1_Seq0_0;
  }
  else if(module <= Adc1_Seq1_3)
  {
    seq = &Adc1S1;
    moduleOffset = Adc1_Seq1_0;
  }
  else if(module <= Adc1_Seq2_3)
  {
    seq = &Adc1S2;
    moduleOffset = Adc1_Seq2_0;
  }
  else if(module <= Adc1_Seq3_0)
  {
    seq = &Adc1S3;
    moduleOffset = Adc1_Seq3_0;
  }

  //if enough time has passed to where we want to take another snapshot with this sequencer,
  //or if the timer has rolled over, then run the adc conversion. Else, the data from the last
  //conversion is fine according to the application. We do this so that the application can take advanatage
  //of the fact that the conversion technically takes the values of several different channels at once to optimize time usage
  uint32_t currentTimestamp = micros();
  if(abs((int64_t)(currentTimestamp - seq->lastTimestamp)) > seqDelayFactor_us || seq->lastTimestamp == 0)
  {
    if(currentTimestamp - seq->lastTimestamp > 1000) //got hung up, restart it and return previous result.
    {
      adcStartConv(*seq);
    }
    if(adcGetConvResult(*seq) == false)
    {
      return ROVEADC_INCOMPLETE_CONVERSION;
    }
    else
    {
      seq->lastTimestamp = currentTimestamp;
    }
  }

  *retBuff = seq->results[module - moduleOffset];
  return ROVEADC_SUCCESS;
}

RoveAdc_Error roveAdc_startConversion(RoveAdc_Handle handle)
{
  if(handle.initialized == false)
  {
   debugFault("roveAdc_startConversion: Handle not initialized");
  }
  seq_conf *seq;

  uint8_t module = handle.index;

  //get adc sequencer information from module
  if(module <= Adc0_Seq0_7)
  {
   seq = &Adc0S0;
  }
  else if(module <= Adc0_Seq1_3)
  {
   seq = &Adc0S1;
  }
  else if(module <= Adc0_Seq2_3)
  {
   seq = &Adc0S2;
  }
  else if(module <= Adc0_Seq3_0)
  {
   seq = &Adc0S3;
  }
  else if(module <= Adc1_Seq0_7)
  {
   seq = &Adc1S0;
  }
  else if(module <= Adc1_Seq1_3)
  {
   seq = &Adc1S1;
  }
  else if(module <= Adc1_Seq2_3)
  {
   seq = &Adc1S2;
  }
  else if(module <= Adc1_Seq3_0)
  {
   seq = &Adc1S3;
  }

  adcStartConv(*seq);
  return ROVEADC_SUCCESS;
}

void setSequenceConversionDeadband(uint32_t deadband_us)
{
  seqDelayFactor_us = deadband_us;
}

float roveAdc_toVolts(uint32_t result)
{
  float _result = result;

  return _result * 3.3 / 4095.0; //voltage = (adc reading) * (system voltage / adc resolution). Adc is 12 bit, so resolution is 2^12 - 1
}

//not currently used
/*
static void adc0Isr0()
{

}
static void adc0Isr1()
{

}
static void adc0Isr2()
{

}
static void adc0Isr3()
{

}
static void adc1Isr0()
{

}
static void adc1Isr1()
{

}
static void adc1Isr2()
{

}
static void adc1Isr3()
{

}*/
