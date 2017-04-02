//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "bms.h"

void TA2_0_IRQHandler(void) //Temp sensor data retrieval
{
    ow_temp_reading.f = read_scratch_singledrop(&ow_temp);
    TA2CCTL0 &= ~CCIFG;
}

void TA2_N_IRQHandler(void) //Start temp sensor measurement
{
    start_conv_singledrop(&ow_temp);
    TA2CCTL1 &= ~CCIFG;
}

void TA1_0_IRQHandler(void) //Assuming ADC conversions won't take 15ms since the clock is 12MHz
{
    ADC14->CTL0 |=
            ADC14_CTL0_ENC |
            ADC14_CTL0_SC;
    TA1CCTL0 &= ~CCIFG;
}
void TA0_0_IRQHandler(void) //Get LTC conv results
{
    //ltc6803_rddgnr(); //read diagnostic regs
    ltc6803_rdcv();
    TA0CCTL0 &= ~CCIFG;
}

void TA0_N_IRQHandler(void) //Start LTC conv
{
    __no_operation();
    TA0CTL &= ~TIMER_A_CTL_MC_MASK; //Under normal circumstances, we don't want this, but here we want to ensure 15ms delay after the -end- of STCV
    ltc6803_stcvad();
    //ltc6803_dagn(); //for diagnostics, should read 2.5V
    TA0CTL |= TIMER_A_CTL_MC1;

    TA0CCTL1 &= ~CCIFG;
}
void ADC14_IRQHandler(void)
{
//We don't need to clear the interrupt flag; the reg reads do that
//Get the data from conversion memory and put it in an array
adc14_out[0] = ADC14->MEM[0]; //PACK_I_MEAS
adc14_out[1] = ADC14->MEM[1]; //V_CHECK_ARRAY
adc14_out[2] = ADC14->MEM[2]; //V_CHECK_OUT
pack_vtg_array.f = (adc14_out[1] * (((1.93634e-5 * adc14_out[1]) + 3.02779)/16384) * 11) -0.7; //Magic number based on multimeter readings and 14-bit scale. Re-test in rover later
pack_vtg_out = adc14_out[2] * (VCC / 16384);
pack_i.f = -(((adc14_out[0] * (VCC / 16384))-SENSOR_BIAS)/SENSOR_SENSITIVITY);
//__no_operation(); //Debugging use
}

void EUSCIA2_IRQHandler() //We received a command
{
EUSCI_A2 -> IFG &= ~BIT0;
pb_command = EUSCI_A2 -> RXBUF;
}

void main(void)
{
	
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    //MAKE SURE you do clk_init first thing. Everything else -- delay cycles, timers, ADC, communication is dependent on it
    clk_init();
    spi_init();
    uart_init();
    //Outputs. The inputs will get set automatically this way.
    P2DIR = (FAN_CTRL_1 | FAN_CTRL_2 | FAN_CTRL_3 | FAN_CTRL_4);
    P4DIR = (LOGIC_SWITCH | PACK_GATE);
    P5DIR = BMS_CSBI;
    P8DIR = (LED_SER_IN | LED_SRCK);
    //TODO: Make list of unused pins to set to output
   /* P1OUT = 0;
    P2OUT = 0;
    P3OUT = 0;
    P4OUT = 0;
    P5OUT = 0;
    P6OUT = 0;
    P7OUT = 0;
    P8OUT = 0;
    P9OUT = 0;
    P10OUT = 0;*/
  /*  ow_temp.port_out = &P3OUT;
    ow_temp.port_in = &P3IN;
    ow_temp.port_ren = &P3REN;
    ow_temp.port_dir = &P3DIR;
    ow_temp.pin = 0;*/
    //Special Functions
    // 4.2, 4.5, 6.0 ADC
    // 3.2, 3.3 UART module
    // 1.5, 1.6, 1.7 SPI

 //3.2 and 3.3 primary special function

    ow_temp.port_in = &P3IN;
    ow_temp.port_out = &P3OUT;
    ow_temp.port_dir = &P3DIR;
    ow_temp.pin = BIT0;

    //Initial pin states
    P5OUT |= BMS_CSBI; //CSBI should be high when a conversion is not in progress

    P4OUT |= PACK_GATE; // turn on the pack
    P4OUT &= ~LOGIC_SWITCH; //Make sure rocker switch is on for BMS logic power (I guess it's active low?)

    ltc6803_wrcfg(CDC0); //Configure LTC in standby mode

    timer_a0_init();
    timer_a1_init();
    timer_a2_init();
    adc14_init();
    write_scratch_singledrop(&ow_temp, 0xFF, 0x00, RES_12);
    pb_command=0;
    ow_temp_reading.f = 0.0;
    //main loop
    //TODO: Put switch in rx interrupt, put CPU in LPM0_LDO_VCORE1
    while(true)
    {
        switch(pb_command)
        {
        case 0: //No operation
            __no_operation();
            break;
        case 1: //Shutdown pack
            P4OUT &= ~PACK_GATE;
            pb_command = 0;
            break;

        case 2: //Reboot pack
            P4OUT &= ~PACK_GATE;
            __delay_cycles(15000000); //5s * 3MHz
            P4OUT |= PACK_GATE;
            pb_command = 0;
            break;

        case 3: //fans on
            P2OUT = (BIT4 | BIT5 | BIT6 | BIT7);
            pb_command = 0;
            break;

        case 4: //fans off
            P2OUT &= ~(BIT4 | BIT5 | BIT6 | BIT7);
            pb_command = 0;
            break;

        case 5: //sound buzzer
            for(j = 0; j<4; j++)
                uart_tx(TARGET_PB, pack_i.ch[j]);
            for(j = 0; j<4; j++)
                uart_tx(TARGET_PB, pack_vtg_array.ch[j]);
            for(j = 0; j<4; j++)
                uart_tx(TARGET_PB, ow_temp_reading.ch[j]);
            tx_cvs();
            pb_command = 0;
            break;

        default: //invalid command
            while(1); // Trap CPU so we can figure out what got sent to cause it. Make sure the final revision just buzzes 3x and returns
            __no_operation();
            break;
        }
    }


}
