//*****************************************************************************
//
// MSP432 BMS Software
//
//****************************************************************************

//TODO: -Convert most things beyond toggling GPIO to driverlib.

#include "bms.h"


void RTC_C_IRQHandler(void)
{
    int i=0;
    RTCIV = 0x0000;
       if (pack_vtg_out < BATTERY_LOW_CRIT) {
          mins++;
          for(i=0; i<7; i++) {
              P5OUT |= BUZZER;
              __delay_cycles(200000);
              P5OUT &= ~BUZZER;
              __delay_cycles(200000);
          }
       }
       else {
                     mins = 0;
       }
       if (mins > IDLE_SHUTOFF_MINS) {
           for(i=0; i<24; i++) {
               P5OUT |= BUZZER;
               __delay_cycles(100000);
               P5OUT &= ~BUZZER;
               __delay_cycles(100000);
           }
           P3OUT |= LOGIC_SWITCH;
       }
}

void TA2_0_IRQHandler(void) { //Temp sensor data retrieval
    ow_temp_reading.f = read_scratch_singledrop(&ow_temp);
    if(ow_temp_reading.f > 50.0) {
        P1OUT |= FAN_CTRL_1 | FAN_CTRL_2;
        P3OUT |= FAN_CTRL_3 | FAN_CTRL_4;
    }
    else if((ow_temp_reading.f < 40.0) && (manual_fans == false)) {
        P1OUT &= ~(FAN_CTRL_1 | FAN_CTRL_2);
        P3OUT &= ~(FAN_CTRL_3 | FAN_CTRL_4);
    }
  //  uart_tx(TARGET_IND, );
    TA2CCTL0 &= ~CCIFG;
}

void TA2_N_IRQHandler(void) { //Start temp sensor measurement
    start_conv_singledrop(&ow_temp);
    TA2CCTL1 &= ~CCIFG;
}

void TA1_0_IRQHandler(void) { //Check if we need to start a cell measurement set, start ADC conv, and stop timer until ADC interrupt to avoid race condition
    conv_counts++;
    if (conv_counts > 29) {
        P4OUT |= ADC_CELL_EN; //Turn on the mux
        cell_v_writelock = true; //start a new set of cell measurements
        conv_counts = 0;
    }
    ADC14->CTL0 |= ADC14_CTL0_ENC;
    ADC14->CTL0 |= ADC14_CTL0_SC;
    TA1CCTL0 &= ~CCIFG;
    TA1CTL &= ~TIMER_A_CTL_MC_MASK; //Will be restarted by the ADC IRQ handler. Ensures the interrupt always fires at the same point relative to the end of a conversion.
    TA1R = 0;
}


void ADC14_IRQHandler(void) {
int i = 0, j = 0;
//We don't need to clear the interrupt flag; the reg reads do that
conv_counts_last = conv_counts;
for(i = 0; i < 4; i++) {
    adc14_out[i] = ADC14->MEM[i]; //PACK_I_MEAS, V_CHECK_ARRAY, V_CHECK_OUT, cell_vtgs[whatever] if applicable (i.e. set of readings is in progress)
}
if(cell_v_writelock) { //If cell_v_writelock is asserted, a set of cell measurements is being taken
    cell_vtgs[current_cell].f = adc14_out[3] * 11 * (VCC / 16384);
    current_cell++;
    if(current_cell > 7) {
        current_cell = 0;
        P4OUT &= ~(ADC_CELL_EN | ADC_CELL_A0 | ADC_CELL_A1 | ADC_CELL_A2);
        cell_v_writelock = false;
        for(j=0; j<8; j++) {
            cell_vtgs_last[j].f = cell_vtgs[j].f;
        }
        __no_operation();
    }
    else {
        P4OUT = (P4OUT & ~(ADC_CELL_A1 | ADC_CELL_A2 | ADC_CELL_A0)) | current_cell; //Mask out the values, then set them back if they're 1
    }
}
pack_vtg_array.f = adc14_out[1] * 11 * (VCC / 16384);
pack_vtg_out = adc14_out[2] * 11 * (VCC / 16384);
pack_i.f = (((adc14_out[0] * (VCC / 16384)) - SENSOR_BIAS)/SENSOR_SENSITIVITY);
if (pack_i.f < 0)
    pack_i.f *=-1;
if ((pack_i.f >= AMP_OVERCURRENT)||(pack_vtg_array.f < 22.0))
{
    P3OUT &= ~PACK_GATE;
    for(i=0; i<8; i++)
    {
        P5OUT |= BUZZER;
        __delay_cycles(1000000);
        P5OUT &= ~BUZZER;
    }
__no_operation(); //Debugging use

} //Comment the above out if you're testing without anything connected, the alarm will trip otherwise
TA1CTL |= TIMER_A_CTL_MC__UP;
}

void EUSCIA2_IRQHandler() //RX command from power board
{
    uint8_t pb_command = 0;
    int j, h;
    EUSCI_A2 -> IFG &= ~BIT0;
    pb_command = EUSCI_A2 -> RXBUF;
    switch(pb_command)
    {
    case 0: //No operation
        __no_operation();
        break;
    case 1: //Shutdown pack
        P3OUT &= ~PACK_GATE;
        pb_command = 0;
        mins = 0;
        break;

    case 2: //Reboot pack
        P3OUT &= ~PACK_GATE;
        __delay_cycles(15000000); //5s * 3MHz
        P3OUT |= PACK_GATE;
        pb_command = 0;
        mins = 0;
        break;

    case 3: //fans on
        P1OUT |= (FAN_CTRL_1 | FAN_CTRL_2);
        P3OUT |= (FAN_CTRL_3 | FAN_CTRL_4);
        manual_fans = true;
        pb_command = 0;
        mins = 0;
        break;

    case 4: //fans off
        P1OUT &= ~(FAN_CTRL_1 | FAN_CTRL_2);
        P3OUT &= ~(FAN_CTRL_3 | FAN_CTRL_4);
        manual_fans = 0;
        pb_command = 0;
        mins = 0;
        break;

    case 5: //PB data request. Order: Pack current, pack voltage, pack temperature, cell voltages(array).
            for(j = 0; j < 4; j++)
                uart_tx(TARGET_PB, pack_i.ch[j]);
            for(j = 0; j < 4; j++)
                uart_tx(TARGET_PB, pack_vtg_array.ch[j]);
            for(j = 0; j < 4; j++)
                uart_tx(TARGET_PB, ow_temp_reading.ch[j]);
            for(j = 0; j < 8; j++) {
                for(h = 0; h < 4; h++) {
                    uart_tx(TARGET_PB, cell_vtgs_last[j].ch[h]);
                }
            }
        pb_command = 0;
        break;

    default: //invalid command
        __no_operation(); //Breakpoint here if you think you're getting invalid commands
        break;
    }
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    //clk_init must be done first for correct operation -- all other functions expect 48MHz.
    clk_init();
    uart_init();

    P1DIR = FAN_CTRL_1 | FAN_CTRL_2;
    P3DIR |= PACK_GATE | LOGIC_SWITCH | FAN_CTRL_3 | FAN_CTRL_4;
    P4DIR |= ADC_CELL_EN | ADC_CELL_A0 | ADC_CELL_A1 | ADC_CELL_A2;
    P5DIR = BUZZER;
    P2DIR |= BIT6;

    P1OUT &= ~(FAN_CTRL_1 | FAN_CTRL_2);
    P3OUT &= ~(FAN_CTRL_3 | FAN_CTRL_4);
    P4OUT &= ~(ADC_CELL_EN | ADC_CELL_A1 | ADC_CELL_A2 | ADC_CELL_A0);
    P5OUT &= ~BUZZER;

 //3.2 and 3.3 primary special function

    ow_temp.port_in = &P2IN;
    ow_temp.port_out = &P2OUT;
    ow_temp.port_dir = &P2DIR;
    ow_temp.pin = BIT5;

    current_cell = 0;
    mins = 0;
    manual_fans = 0;
    conv_counts = 0;
    conv_counts_last = 0;


    P3OUT |= PACK_GATE; // turn on the pack
    P3OUT &= ~LOGIC_SWITCH; //Make sure rocker switch is on for BMS logic power (I guess it's active low?)

    timer_a1_init();
    timer_a2_init();
    adc14_init();
    rtc_init();
    write_scratch_singledrop(&ow_temp, 0xFF, 0x00, RES_12);
    ow_temp_reading.f = 0.0;
    for(j=0; j<8; j++) {
        cell_vtgs_last[j].f = 0;
    }
    //main loop
    while(true)
    {
      PCM_gotoLPM0InterruptSafe();
    }
}
