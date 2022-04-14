// Battery Managment System (BMS) Software System
// Main cpp file (Needed to clarify that this is in c++)

// The BMS System is created for the Missouri S&T 2022 Competition Rover.

// created by Sean Duda

// Included Libraries

#include BMS_Software.h // this is a main header file for the BMS.
// other code might possibly need to be included ask Anthony about files on Github. Include screenshot.

// Setup and Main Loop

uint8_t error_report[RC_BMSBOARD_ERROR_DATACOUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
bool pinfault_state = false;
int num_loop = 0; // battery temperature sensor state defualt to false in case of an error report
bool sw_ind_state = false;
uint32_t time_since_Rovecomm_update = 0; // intial time of Rovecomm updating
uint32_t meas_batt_temp[NUM_TEMP_AVERAGE];

void setup()

{
    Serial.begin(9600);
    Serial3.begin(9600);
    Serial3.begin(9600);
    startScreen(); // Rovecomm communications of BMS start up
    RoveComm.begin(RC_BMSBOARD_FOURTHOCTET);
    delay(ROVECOMM_DELAY);

    setInputPins();
    setOutputPins();
    setOutputStates();

    // //Serial.println("Setup Complete.");              <----------- What is going on here? Should this be included or was it considered?
} // end setup

void loop() // object identifier loop when called id or loop it runs?
{
    int32_t main_current;
    uint16_t cell_voltages[RC_BMSBOARD_VMEASmV_DATACOUNT]; // identifying main current, pack voltage out, and battery temp.
    int pack_out_voltage;
    uint32_t batt_temp;
    rovecomm_packet packet;

    // Serial.println();
    getMainCurrent(main_current); // functions to retrieve current, Low, and high cell voltages, and battery temp.
    reactOverCurrent();

    getCellVoltage(cell_voltages);  // I am assuming react is a function that will terminate the loop if a problem stated occurs
    reactUnderVoltage();            // Problems stated include: undervolting batteries, low voltage, Estop pressed, ---
    reactLowVoltage(cell_voltages); // Forgotten Logic Switch, and Over temperature.

    getOutVoltage(pack_out_voltage);
    reactEstopReleased();
    reactForgottenLogicSwitch();

    getBattTemp(batt_temp);
    reactOverTemp();

    if ((millis() - time_since_Rovecomm_update) >= ROVECOMM_UPDATE_DELAY)
    {
        // batt_temp = 23456;//just for fixin reds temp values
        ////Serial.println(batt_temp);
        RoveComm.write(RC_BMSBOARD_MAINIMEASmA_HEADER, main_current);
        delay(ROVECOMM_DELAY);
        RoveComm.write(RC_BMSBOARD_VMEASmV_HEADER, cell_voltages); // this if statement is created to write information of current, cell volt.,
        delay(ROVECOMM_DELAY);                                     // and battery temperature if the update delay is greater or equal to time since last update
        RoveComm.write(RC_BMSBOARD_TEMPMEASmDEGC_HEADER, batt_temp);
        delay(ROVECOMM_DELAY);

        if (pinfault_state == true)
        {
            RoveComm.write(RC_BMSBOARD_ERROR_HEADER, error_report); // pin fault orginally defaults to false, but if true an error has occured with Rovecom update
            delay(DEBOUNCE_DELAY);                                  // don't know what debounce delay is or does
        }

        time_since_Rovecomm_update = millis();
    }

    packet = RoveComm.read();
    if (packet.data_id != 0)
    {
        switch (packet.data_id) // don't understand packets and I don't remember how to use a swtich case.
        {
        case RC_BMSBOARD_SWESTOPs_DATAID:
        {
            setEstop(packet.data[0]);
            break;
        }
            // expand by adding more cases if more dataids are created
        } // end switch
    }     // end if

    if ((num_loop % UPDATE_ON_LOOP) == 0) // SW_IND led will blink while the code is looping and LCD will update
    {
        if (sw_ind_state == false)
        {
            digitalWrite(SW_IND_PIN, HIGH); // High function will power LED on
            sw_ind_state = true;
        } // end if
        else
        {
            digitalWrite(SW_IND_PIN, LOW); // Low function will have power of LED off.
            sw_ind_state = false;
        } // end if
        updateLCD(batt_temp, cell_voltages);
    }           // end if
    num_loop++; // incrementing loop

} // end loop

// Static Variables for Below Functions /////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current
static int num_overcurrent = 0;
static bool overcurrent_state = false;
static float time_of_overcurrent = 0;

// Voltage
static bool pack_undervoltage_state = false;
static bool cell_undervoltage_state = false;
static bool low_voltage_state = false;
static int num_low_voltage_reminder = 0;
static int time_of_low_voltage = 0;

// Temp
static int num_meas_batt_temp = 0;
static bool batt_temp_avail = false;
static bool overtemp_state = false;
static bool fans_on = false;

// Logic Switch
static bool forgotten_logic_switch = false;
static int num_out_voltage_loops = 0;
static int time_switch_forgotten = 0;
static int time_switch_reminder = 0;
static bool estop_released_beep = false;

// Functions /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setInputPins()
{
    pinMode(PACK_I_MEAS_PIN, INPUT); // pack current sensor
    pinMode(PACK_V_MEAS_PIN, INPUT); // pack voltage sensor
    pinMode(LOGIC_V_MEAS_PIN, INPUT);
    pinMode(TEMP_degC_MEAS_PIN, INPUT); // temperature sensor
    pinMode(C1_V_MEAS_PIN, INPUT);
    pinMode(C2_V_MEAS_PIN, INPUT);
    pinMode(C3_V_MEAS_PIN, INPUT);
    pinMode(C4_V_MEAS_PIN, INPUT); // Connected cells to pins to measure individual cell data
    pinMode(C5_V_MEAS_PIN, INPUT);
    pinMode(C6_V_MEAS_PIN, INPUT);
    pinMode(C7_V_MEAS_PIN, INPUT);
    pinMode(C8_V_MEAS_PIN, INPUT);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setOutputPins() // output pin functions
{
    pinMode(PACK_OUT_CTR_PIN, OUTPUT);
    pinMode(LOGIC_SWITCH_CTR_PIN, OUTPUT);
    pinMode(BUZZER_CTR_PIN, OUTPUT);
    pinMode(FAN_1_CTR_PIN, OUTPUT); // Changed to one fan control pin, to reduce redundancy and have all fans powered on at once through an 8 pin molex connector
    pinMode(FAN_PWR_IND_PIN, OUTPUT);
    pinMode(SW_IND_PIN, OUTPUT); // indicator pins
    pinMode(SW_ERR_PIN, OUTPUT);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setOutputStates()
{
    digitalWrite(PACK_OUT_CTR_PIN, HIGH);    // digital write is used to write high and low values of digital pins
    digitalWrite(LOGIC_SWITCH_CTR_PIN, LOW); // 5V - 3.3V is low while anything above 5V is considered to be high
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(FAN_1_CTR_PIN, LOW); // Only one digital write is needed to power the fans of the 8 pin molex connector
    digitalWrite(FAN_PWR_IND_PIN, LOW);
    digitalWrite(SW_IND_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getMainCurrent(int32_t &main_current)
{
    ////Serial.print("adc current:  ");
    ////Serial.println(analogRead(PACK_I_MEAS_PIN));

    main_current = ((map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX) * 1069) / 1000); //*950)/1000);

    // display of current functions

    ////Serial.print("current:  ");
    ////Serial.println(main_current);

    if (main_current > OVERCURRENT)
    {
        delay(DEBOUNCE_DELAY); // debounce delay is to check twice in a short period of time
        if (map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX) > OVERCURRENT)
        {
            overcurrent_state = true;

            if (num_overcurrent == 1)
            {
                num_overcurrent++; // loop rerun with increments
            }                      // end if
        }                          // end if
    }                              // end if
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getCellVoltage(uint16_t cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT])
{
    pinfault_state = false; // cell voltage pin reader if the pinfualt state is false then the code will continue
                            // Serial.println();
    ////Serial.println("///////////////////Cell values/////////////////////// ");
    for (int i = 0; i < RC_BMSBOARD_VMEASmV_DATACOUNT; i++)
    {
        //////////////////////PACK/////////////////////////////////
        if (i == RC_BMSBOARD_VMEASmV_PACKENTRY)
        {
            int adc_read = analogRead(CELL_MEAS_PINS[RC_BMSBOARD_VMEASmV_PACKENTRY]); // adc means an analog to digital converter
                                                                                      // Serial.print("adc log voltage : ");
            // Serial.println(adc_read);

            cell_voltage[RC_BMSBOARD_VMEASmV_PACKENTRY] = (/*1095*/ 1215 * map(adc_read, PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX) / 1000); // TODO: Fix voltage divider for pack meas so that we can remove this weird scaling.
            error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_NOERROR;
            // Serial.print("mapped log voltage : ");
            // Serial.println(cell_voltage[RC_BMSBOARD_VMEASmV_PACKENTRY]);

            if ((cell_voltage[RC_BMSBOARD_VMEASmV_PACKENTRY] > PACK_SAFETY_LOW) && // if statement of Remote cont. pack entry parameters
                (cell_voltage[RC_BMSBOARD_VMEASmV_PACKENTRY] < PACK_UNDERVOLTAGE))
            {

                delay(DEBOUNCE_DELAY);

                if ((((1094 * map(analogRead(CELL_MEAS_PINS[RC_BMSBOARD_VMEASmV_PACKENTRY]), PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)) / 1000) < PACK_UNDERVOLTAGE) && (((1094 * map(analogRead(CELL_MEAS_PINS[RC_BMSBOARD_VMEASmV_PACKENTRY]), PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)) / 1000) > PACK_SAFETY_LOW))
                {
                    pack_undervoltage_state = true;
                    error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_UNDERVOLTAGE;
                } // end if
            }     // end if
            if (cell_voltage[RC_BMSBOARD_VMEASmV_PACKENTRY] < PACK_EFFECTIVE_ZERO)
            {
                error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_PINFAULT; // effectively recieving no voltage to pin resulting in error
                pinfault_state = true;                                                  // should the pinfault of the error statement be above the report?
            }                                                                           // end if
        }                                                                               // end if

        //////////////////////////CELLS//////////////////////////////

        if (i > RC_BMSBOARD_VMEASmV_PACKENTRY) // i is an incrementing integer that will increment per loop given with for statement above
        {
            int adc_reading = analogRead(CELL_MEAS_PINS[i]);
            // Serial.print("adc reading : ");
            // Serial.println(adc_reading);
            if (adc_reading < CELL_V_ADC_MIN)
            {
                adc_reading = CELL_V_ADC_MIN; // process to rewrite voltage as ac to dc converter min, and max values
            }                                 // end if
            if (adc_reading > CELL_V_ADC_MAX)
            {
                adc_reading = CELL_V_ADC_MAX;
            }                                                                                                                     // end if////////////////HERE/////////////////////////////////////////////////////////////////
            cell_voltage[i] = ((map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX)) * 1030) / 1000; // 980
            if (i > 2)
                cell_voltage[i] -= 00; // cell voltage less than or equal to zero if i > 2 after incrementation through loop.
                                       // Serial.print("cell voltage : ");
            // Serial.println(cell_voltage[i]);

            error_report[i] = RC_BMSBOARD_ERROR_NOERROR;

            if ((cell_voltage[i] > CELL_VOLTS_MIN) && (cell_voltage[i] < CELL_UNDERVOLTAGE))
            {
                delay(DEBOUNCE_DELAY);

                adc_reading = analogRead(CELL_MEAS_PINS[i]); // an analog signal creates a varying signal source

                if (adc_reading < CELL_V_ADC_MIN)
                {
                    adc_reading = CELL_V_ADC_MIN;
                } // end if
                if (adc_reading > CELL_V_ADC_MAX)
                {
                    adc_reading = CELL_V_ADC_MAX;
                }                                                                                                           // end if
                if ((map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX) <= CELL_UNDERVOLTAGE) // remap of new data values found of ac to dc
                    && (map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX) > CELL_VOLTS_MIN))
                {
                    cell_undervoltage_state = true;

                    error_report[i] = RC_BMSBOARD_ERROR_UNDERVOLTAGE;

                } // end if
            }     // end if
            if (adc_reading == CELL_V_ADC_MIN)
            {
                error_report[i] = RC_BMSBOARD_ERROR_PINFAULT;
                pinfault_state = true;
            } // end if
        }     // end if
    }         // end for
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getOutVoltage(int &pack_out_voltage)
{
    int adc_reading = analogRead(PACK_V_MEAS_PIN);
    ////Serial.print("adc vout : ");
    ////Serial.println(adc_reading);
    ////////////////////////////////////HERE////////////////////////////////////////////////
    pack_out_voltage = ((1269 * map(adc_reading, PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)) / 1000); // previously1369
    ////Serial.print("mapped vout : ");
    ////Serial.println(pack_out_voltage);
    if (pack_out_voltage > PACK_SAFETY_LOW)
    {
        forgotten_logic_switch = false; // logic switch isn't forgotten and a value is associated with time switch and number of volt. loops
        time_switch_forgotten = 0;
        num_out_voltage_loops = 0;
    } // end if
    if (pack_out_voltage < PACK_EFFECTIVE_ZERO)
    {
        delay(DEBOUNCE_DELAY);

        if (((1269 * map(analogRead(PACK_V_MEAS_PIN), PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)) / 1000) < PACK_EFFECTIVE_ZERO)
        {
            forgotten_logic_switch = true; // if the data values of adc are less than pack effective then logic switch is forgotten and estop switch is prevented from triggered
            estop_released_beep = false;
            num_out_voltage_loops++; // incrementing number of voltage loops if pack out voltage less than effective zero and parameters are not met of current if loop
        }                            // end if
    }                                // end if
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getBattTemp(uint32_t &batt_temp)
{
    int adc_reading = analogRead(TEMP_degC_MEAS_PIN);
    if (adc_reading > TEMP_ADC_MAX)
    {
        adc_reading = TEMP_ADC_MAX; // analog pin rewrite with new parameters of given if statements from adc
    }
    if (adc_reading < TEMP_ADC_MIN)
    {
        adc_reading = TEMP_ADC_MIN;
    }

    meas_batt_temp[num_meas_batt_temp] = (1060 * (map(adc_reading, TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX)) / 1000); // function of measuring battery temp with
    num_meas_batt_temp++;                                                                                                    // increasing interval of the function

    batt_temp = (925 * (map(analogRead(TEMP_degC_MEAS_PIN), TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX)) / 1000); // mapping analog signals of battery temp data

    if (num_meas_batt_temp % NUM_TEMP_AVERAGE == 0)
    {
        for (int i = 0; i < NUM_TEMP_AVERAGE; i++)
        {
            batt_temp += meas_batt_temp[i];
        }                              // end for
        batt_temp /= NUM_TEMP_AVERAGE; // batt_temp is the average of all the measurments in the meas_batt_temp[] array. Giving the total average battery temperature.
        num_meas_batt_temp = 0;
        batt_temp_avail = true; // Set to true after first batt_temp value is avail. Avoids acting on overtemp before the first average is computed.

        // Serial.print("batt_temp: ");
        // Serial.println(batt_temp);
    } // end if

    if (batt_temp_avail == true)
    {
        if (batt_temp < TEMP_THRESHOLD) // if statement to check if battery temperature is over the max amount.
        {
            overtemp_state = false;
        } // end if
        if (batt_temp > TEMP_THRESHOLD)
        {
            delay(DEBOUNCE_DELAY);

            if (map(analogRead(TEMP_degC_MEAS_PIN), TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX) > TEMP_THRESHOLD)
            {
                overtemp_state = true;
            } // end if
        }     // end if
    }         // end if

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void updateLCD(int32_t batt_temp, uint16_t cellVoltages[])
{
    // Serial.print("**************************************************************************");

    const int NUM_CELLS = 8; // this should be in header?
    bool LCD_Update = false;
    float time1 = 0;
    float packVoltage = 0; // V not mV
    float packTemp = 0;    // degC

    packVoltage = (static_cast<float>(cellVoltages[0]) / 1000);
    // Serial.print("temp   ");
    // Serial.println(batt_temp);
    packTemp = (static_cast<float>(batt_temp) / 1000);
    packTemp = roundf(packTemp * 10) / 10; // Rounds temp to closest tenth of a degree

    if (LCD_Update == false)
    {
        time1 = millis();
        LCD_Update = true;
    }

    // Send the clear command to the display - this returns the cursor to the beginning of the display
    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display

    Serial3.print("Pack:");
    Serial3.print(packVoltage, 1); // display packVoltage from BMS, in V
    Serial3.print("V");
    Serial3.print(" Temp:");
    Serial3.print(packTemp, 1); // display packTemp from BMS, in C
    Serial3.print("C");

    for (int i = 0; i < NUM_CELLS; i++)
    {
        float temp_cell_voltage = 0;
        temp_cell_voltage = (static_cast<float>(cellVoltages[i + 1]) / 1000);
        Serial3.print(i + 1); // Printing cell numbers with increased value.
        Serial3.print(":");
        Serial3.print(temp_cell_voltage, 1); // cellVoltage from BMS in V?  shows one decimal place
        if ((i + 1) % 3 == 0)                // modular division of 3
        {
            Serial3.print("V");
        } // end if
        else
        {
            Serial3.print("V ");
        } // end else
    }     // end for
    LCD_Update = false;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactOverCurrent()
{
    if (overcurrent_state == false)
    {
        error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_OVERCURRENT;
    } // end if
    if (overcurrent_state == true)
    {
        if (num_overcurrent == 0)
        {
            error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_OVERCURRENT;
            RoveComm.write(RC_BMSBOARD_ERROR_HEADER, error_report);
            delay(ROVECOMM_DELAY);

            digitalWrite(PACK_OUT_CTR_PIN, LOW);

            time_of_overcurrent = millis();
            num_overcurrent++;

            notifyOverCurrent();
        } // end if
        else if (num_overcurrent == 1)
        {
            if (millis() >= (time_of_overcurrent + RESTART_DELAY))
            {
                digitalWrite(PACK_OUT_CTR_PIN, HIGH);
            } // end if
            if (millis() >= (time_of_overcurrent + RESTART_DELAY + RECHECK_DELAY))
            {
                overcurrent_state = false;
                num_overcurrent = 0;
                time_of_overcurrent = 0;
            } // end if
        }     // end else if

        else
        {
            error_report[RC_BMSBOARD_ERROR_PACKENTRY] = RC_BMSBOARD_ERROR_OVERCURRENT;
            RoveComm.write(RC_BMSBOARD_ERROR_HEADER, error_report);
            delay(ROVECOMM_DELAY);

            digitalWrite(PACK_OUT_CTR_PIN, LOW);

            notifyOverCurrent();

            digitalWrite(LOGIC_SWITCH_CTR_PIN, HIGH); // BMS Suicide
        }                                             // end else
    }                                                 // end if
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactUnderVoltage()
{
    ////Serial.println("reactUnderVoltage");
    if ((pack_undervoltage_state == true) || cell_undervoltage_state == true)
    {
        RoveComm.write(RC_BMSBOARD_ERROR_HEADER, error_report);
        delay(DEBOUNCE_DELAY);

        digitalWrite(PACK_OUT_CTR_PIN, LOW);
        notifyUnderVoltage();
        digitalWrite(LOGIC_SWITCH_CTR_PIN, HIGH); // BMS Suicide
    }                                             // end if
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactOverTemp()
{
    if (overtemp_state == true && fans_on == false)
    {
        fans_on = true;
        digitalWrite(FAN_1_CTR_PIN, HIGH); // only one fan control is used to control all 4 fans colletively.
        digitalWrite(FAN_PWR_IND_PIN, HIGH);
    } // end if
    if (overtemp_state == false && fans_on == true)
    {
        fans_on = false;
        digitalWrite(FAN_1_CTR_PIN, LOW);
        digitalWrite(FAN_PWR_IND_PIN, LOW);
    } // end if
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactForgottenLogicSwitch()
{
    if (forgotten_logic_switch == true)
    {
        if (num_out_voltage_loops == 1)
        {
            time_switch_forgotten = millis();
            time_switch_reminder = millis();
        } // end if
        if (num_out_voltage_loops > 1)
        {
            if (millis() >= time_switch_reminder + LOGIC_SWITCH_REMINDER)
            {
                time_switch_reminder = millis();
                notifyLogicSwitch();
            } // end if
            if (millis() >= time_switch_forgotten + IDLE_SHUTOFF_TIME)
            {
                digitalWrite(LOGIC_SWITCH_CTR_PIN, HIGH); // BMS Suicide
            }                                             // end if
        }                                                 // end if
    }                                                     // end if
    return;
} // end func

void reactEstopReleased()
{
    if (forgotten_logic_switch == false && estop_released_beep == false)
    {
        estop_released_beep = true;
        notifyEstopReleased();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactLowVoltage(uint16_t cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT])
{
    ////Serial.println("reactLowVoltage");
    if ((cell_voltage[0] > PACK_UNDERVOLTAGE) && (cell_voltage[0] <= PACK_LOWVOLTAGE) && (low_voltage_state = false)) // first instance of low voltage
    {
        low_voltage_state = true;
        notifyLowVoltage();
        time_of_low_voltage = millis();
        num_low_voltage_reminder = 1;
    }                                                                                                                     // end if
    else if ((cell_voltage[0] > PACK_UNDERVOLTAGE) && (cell_voltage[0] <= PACK_LOWVOLTAGE) && (low_voltage_state = true)) // following instances of low voltage
    {
        if (millis() >= (time_of_low_voltage + (num_low_voltage_reminder * LOGIC_SWITCH_REMINDER)))
        {
            notifyLowVoltage();
            num_low_voltage_reminder++;
        } // end if
    }     // end else if
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setEstop(uint8_t data)
{
    if (data == 0)
    {
        digitalWrite(PACK_OUT_CTR_PIN, LOW);

        notifyEstop();

        digitalWrite(LOGIC_SWITCH_CTR_PIN, HIGH); // BMS Suicide
        // If BMS is not turned off here, the PACK_OUT_CTR_PIN would be low and there would be no way to get it high again without reseting BMS anyway.
    } // end if
    else
    {
        digitalWrite(PACK_OUT_CTR_PIN, LOW);
        notifyReboot();
        delay(data * 1000); // Receiving delay in seconds so it needs to be converted to msec.

        digitalWrite(PACK_OUT_CTR_PIN, HIGH);
    } // end else
    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyEstop() // Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(25);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyLogicSwitch() // Buzzer sound: beeep beeep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyEstopReleased() // Buzzer sound: beep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(75);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

void notifyReboot() // Buzzer sound: beeeeeeeeeep beeep beeep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(25);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(25);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(100);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyOverCurrent() // Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(3000);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyUnderVoltage() // Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(150);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(150);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(150);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(150);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(2000);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyLowVoltage() // Buzzer Sound: beeep beeep beeep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(250);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);
    delay(250);

    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
} // end func

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void startScreen()
{

    // put your main code here, to run repeatedly:
    delay(350);
    // Send the clear command to the display - this returns the cursor to the beginning of the display
    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display

    /*Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);*/

    // movingRover();
    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display
    for (int i = 0; i < 4; i++)
    {
        asterisks();
        stars();
    }
    delay(10);

    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display
}

void movingRover()
{
    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display
    for (int i = 0; i < 14; i++)
    {
        delay(450);
        // delay(200);
        // Serial3.write(0x20);
        // Serial3.write(0x20);

        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);

        Serial3.write(0xA1);

        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);

        Serial3.write(0xDB);
        Serial3.write(0xBA);
        Serial3.write(0xDA);
        Serial3.write(0xCD);

        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);

        Serial3.write(0x6F);
        Serial3.write(0x5E);
        Serial3.write(0x6F);
        Serial3.write(0x5E);
        Serial3.write(0x6F);

        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        Serial3.write(0x20);
        /*
          Serial3.write(0x23);//#
          Serial3.write(0x52);//R
          Serial3.write(0x4F);//O
          Serial3.write(0x56);//V
          Serial3.write(0x45);//E
          Serial3.write(0x53);//S
          Serial3.write(0x4F);//O
          Serial3.write(0x48);//H
          Serial3.write(0x41);//A
          Serial3.write(0x52);//R
          Serial3.write(0x44);//D*/
    }
    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display
}

void asterisks()
{
    Serial3.write('|');  // Setting character
    Serial3.write('-');  // Clear display
    Serial3.write(0x56); // V
    Serial3.write(0x41); // A
    Serial3.write(0x4C); // L
    Serial3.write(0x4B); // K
    Serial3.write(0x59); // Y
                         ////////////////////////////////////////////////////////////////need a y
    Serial3.write(0x52); // R
    Serial3.write(0x49); // I
    Serial3.write(0x45); // E
    Serial3.write(0x20);
    Serial3.write(0x49); // I
    Serial3.write(0x4E); // N
    Serial3.write(0x49); // I
    Serial3.write(0x54); // T
    Serial3.write(0x49); // I
    Serial3.write(0x41); // A
    Serial3.write(0x4C); // L
    Serial3.write(0x49); // I
    Serial3.write(0x5A); // Z
    Serial3.write(0x49); // I
    Serial3.write(0x4E); // N
    Serial3.write(0x47); // G

    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x20);

    Serial3.write(0xA1);

    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0xDB);
    Serial3.write(0xBA);
    Serial3.write(0xDA);
    Serial3.write(0xCD);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x6F);
    Serial3.write(0x5E);
    Serial3.write(0x6F);
    Serial3.write(0x5E);
    Serial3.write(0x6F);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    // Serial3.write(0x2A);

    delay(500);

    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display
}

void stars()
{
    Serial3.write('|');  // Setting character
    Serial3.write('-');  // Clear display
    Serial3.write(0x56); // V
    Serial3.write(0x41); // A
    Serial3.write(0x4C); // L
    Serial3.write(0x4B); // K
    Serial3.write(0x59); // Y
    ////////////////////////////////////////////////////////////////need a y
    Serial3.write(0x52); // R
    Serial3.write(0x49); // I
    Serial3.write(0x45); // E
    Serial3.write(0x20);
    Serial3.write(0x49); // I
    Serial3.write(0x4E); // N
    Serial3.write(0x49); // I
    Serial3.write(0x54); // T
    Serial3.write(0x49); // I
    Serial3.write(0x41); // A
    Serial3.write(0x4C); // L
    Serial3.write(0x49); // I
    Serial3.write(0x5A); // Z
    Serial3.write(0x49); // I
    Serial3.write(0x4E); // N
    Serial3.write(0x47); // G

    Serial3.write(0x20);
    Serial3.write(0x2B);

    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0xA1);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0xDB);
    Serial3.write(0xBA);
    Serial3.write(0xDA);
    Serial3.write(0xCD);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x2A);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x20);
    Serial3.write(0x6F);
    Serial3.write(0x5E);
    Serial3.write(0x6F);
    Serial3.write(0x5E);
    Serial3.write(0x6F);
    Serial3.write(0x20);
    Serial3.write(0x2B);
    // Serial3.write(0x2B);

    delay(500);

    Serial3.write('|'); // Setting character
    Serial3.write('-'); // Clear display
}