// Battery Managment System (BMS) Software System

// The BMS System is created for the Missouri S&T 2022 Competition Rover.
// created by Sean Duda， Sanfan Liu
// Included Libraries

#include "BMS_Software.h" // this is a main header file for the BMS.

// rovecomm and packet instances
RoveCommEthernet RoveComm;
rovecomm_packet packet;

// declare the Ethernet Server in the top level sketch with the requisite port ID any time you want to use RoveComm
EthernetServer TCPServer(RC_ROVECOMM_BMSBOARD_PORT);



void setup()
{
    // start up serial communication
    Serial.begin(9600);
    Telemetry.begin(telemetry, 150000);
    startScreen();

    // initialize the ethernet device and rovecomm instance
    RoveComm.begin(RC_BMSBOARD_FOURTHOCTET, &TCPServer, RC_ROVECOMM_BMSBOARD_MAC);
    delay(100);

    setInputPins();
    setOutputPins();
    setOutputStates();

    // //Serial.println("Setup Complete.");              <----------- What is going on here? Should this be included or was it considered?
}

void loop() // object identifier loop when called id or loop it runs?
{
    rovecomm_packet packet;

    // Serial.println();
    getMainCurrent(main_current); // functions to retrieve current, Low, and high cell voltages, and battery temp.
    reactOverCurrent();

    getCellVoltage(cell_voltages);  // I am assuming react is a function that will terminate the loop if a problem stated occurs
    reactUnderVoltage();            // Problems stated include: undervolting batteries, low voltage, Estop pressed, ---
    reactLowVoltage(cell_voltages); // Forgotten Logic Switch, and Over temperature.

    getPackVoltage(pack_out_voltage);
    reactEstopReleased();
    reactForgottenLogicSwitch();

    getBattTemp(batt_temp);
    reactOverTemp();

    packet = RoveComm.read();
    if (packet.data_id != 0)
    {
        switch (packet.data_id) //
        {
            case RC_BMSBOARD_BMSSTOP_DATA_ID:
            {
                setEstop(packet.data[0]);
                break;
            }   
        }
    }

    if (num_loop == UPDATE_ON_LOOP) // SW_IND led will blink while the code is looping and LCD will update
    {
        if (sw_ind_state == false)
        {
            digitalWrite(SW_IND_PIN, HIGH); // High function will power LED on
            sw_ind_state = true;
        }
        else
        {
            digitalWrite(SW_IND_PIN, LOW); // Low function will have power of LED off.
            sw_ind_state = false;
        }
        updateLCD(batt_temp, cell_voltages);
        num_loop = 0;
    }
    num_loop++; // incrementing loop

}

// Static Variables for Below Functions /////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Current
static int num_overcurrent = 0; // if an overcurrent occurs once, it is 1; if it happens twice within some time period: it's greater than 1, BMS suicide
static bool packOverCurrent_state = false;
static float time_of_overcurrent = 0;

// Voltage
static bool pack_undervoltage_state = false;
static uint8_t cell_undervoltage_state = 0;
static uint8_t cell_undervoltage_count = 0;
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
    pinMode(PACK_I_MEAS_PIN, INPUT);    // pack current sensor
    pinMode(PACK_V_MEAS_PIN, INPUT);    // pack voltage sensor
    pinMode(TEMP_degC_MEAS_PIN, INPUT); // temperature sensor
    pinMode(C1_V_MEAS_PIN, INPUT);      // cell voltage sensor
    pinMode(C2_V_MEAS_PIN, INPUT);
    pinMode(C3_V_MEAS_PIN, INPUT);
    pinMode(C4_V_MEAS_PIN, INPUT);
    pinMode(C5_V_MEAS_PIN, INPUT);
    pinMode(C6_V_MEAS_PIN, INPUT);
    pinMode(C7_V_MEAS_PIN, INPUT);
    pinMode(C8_V_MEAS_PIN, INPUT);

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setOutputPins() // output pin functions
{
    pinMode(BUZZER_CTR_PIN, OUTPUT); // buzzer control pin
    pinMode(FAN_CTR_PIN, OUTPUT);    // all-fan control  pin
    pinMode(SER_TX_IND, OUTPUT);     // LCD communication pin
    pinMode(SW_IND_PIN, OUTPUT);     // software indicator pin
    pinMode(SW_ERR_PIN, OUTPUT);     // software error pin

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setOutputStates()
{
    digitalWrite(BUZZER_CTR_PIN, LOW); // turn off buzzer
    digitalWrite(FAN_CTR_PIN, LOW);    // turn off fan
    digitalWrite(SER_TX_IND, LOW);     // turn off LCD communication
    digitalWrite(SW_IND_PIN, LOW);     // turn off software indicator LED
    digitalWrite(SW_ERR_PIN, LOW);     // turn off software error LED

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getMainCurrent(float &main_current)
{
    ////Serial.print("adc current:  ");
    ////Serial.println(analogRead(PACK_I_MEAS_PIN));

    main_current = map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX); // fetch pack current
    // display of current functions
    ////Serial.print("current:  ");
    ////Serial.println(main_current);

    if (main_current > OVERCURRENT) // check current > overcurrent, if not return
    {
        delay(DEBOUNCE_DELAY);                                                                                       // debounce delay is to check twice in a short period of time
        main_current = map(analogRead(PACK_I_MEAS_PIN), CURRENT_ADC_MIN, CURRENT_ADC_MAX, CURRENT_MIN, CURRENT_MAX); // fetch again to double check value
        if (main_current > OVERCURRENT)                                                                              // if current > overcurrent, send out a warning
        {
            // send error flag to roveComm for package over current
            packOverCurrent_state = true;
        }
        else
        {
            packOverCurrent_state = false;
        }
    }
    else
    {
        packOverCurrent_state = false;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getCellVoltage(float CELL_MEAS_PINS[])
{
    pinfault_state = false; // cell voltage pin reader if the pinfualt state is false then the code will continue
    cell_undervoltage_state = 0;
    cell_undervoltage_count = 0;
    // Serial.println();
    ////Serial.println("///////////////////Cell values/////////////////////// ");

    for (int i = 0; i < CELL_COUNT; i++) // loop for the number of batteri
    {
        float adc_reading = analogRead(CELL_MEAS_PINS[i]);
        // Serial.print("adc reading : ");
        // Serial.println(adc_reading);

        if (adc_reading < CELL_V_ADC_MIN) // handle if reading between 0V and 2.4V: lower than expecting power
        {
            adc_reading = CELL_V_ADC_MIN;
        }
        else if (adc_reading > CELL_V_ADC_MAX) // handle if reading above 3.3V	:above expecting voltage
        {
            adc_reading = CELL_V_ADC_MAX;
        }
        cell_voltages[i] = ((map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX))); // map ADC value to Volts
        // Serial.print("cell voltage : ");
        // Serial.println(cell_voltage[i]);
        if ((cell_voltages[i] > CELL_VOLTS_MIN) && (cell_voltages[i] < CELL_UNDERVOLTAGE)) // if between 2.4V and 2.65V
        {
            delay(DEBOUNCE_DELAY); // double check

            adc_reading = analogRead(CELL_MEAS_PINS[i]); // an analog signal creates a varying signal source

            if (adc_reading < CELL_V_ADC_MIN) // handle if reading between 0V and 2.4V: lower than expecting power
            {
                adc_reading = CELL_V_ADC_MIN;
            }
            else if (adc_reading > CELL_V_ADC_MAX) // handle if reading above 3.3V	:above expecting voltage
            {
                adc_reading = CELL_V_ADC_MAX;
            }
            cell_voltages[i] = ((map(adc_reading, CELL_V_ADC_MIN, CELL_V_ADC_MAX, CELL_VOLTS_MIN, CELL_VOLTS_MAX)));
            if ((cell_voltages[i] <= CELL_UNDERVOLTAGE) && (cell_voltages[i] > CELL_VOLTS_MIN)) // map and then compare to min and max expecting voltage
            {
                cell_undervoltage_state |= (1 << i);
                cell_undervoltage_count++;
            }
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getPackVoltage(float &pack_out_voltage)
{
    int adc_reading = analogRead(PACK_V_MEAS_PIN);
    ////Serial.print("adc vout : ");
    ////Serial.println(adc_reading);
    ////////////////////////////////////HERE////////////////////////////////////////////////
    pack_out_voltage = ((VOLTAGE_TO_SIGNAL_RATIO * map(adc_reading, PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)) / 1000); // 1269 is the voltage to analog signal ratio
    ////Serial.print("mapped vout : ");
    ////Serial.println(pack_out_voltage);
    if (pack_out_voltage < PACK_UNDERVOLTAGE)
    {
        delay(DEBOUNCE_DELAY);
        adc_reading = analogRead(PACK_V_MEAS_PIN);
        pack_out_voltage = ((VOLTAGE_TO_SIGNAL_RATIO * map(adc_reading, PACK_V_ADC_MIN, PACK_V_ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX)) / 1000); // previously1369
        if (pack_out_voltage < PACK_UNDERVOLTAGE)
        {
            // raise a error flag for RoveComm pack under voltage
            pack_undervoltage_state = true;
        }
        else
        {
            pack_undervoltage_state = false;
        }
    }
    else
    {
        pack_undervoltage_state = false;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getBattTemp(float &batt_temp)
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

    meas_batt_temp[num_meas_batt_temp] = (MEAS_BATT_TEMP_CONST * (map(adc_reading, TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX)) / 1000); // function of measuring battery temp with
    num_meas_batt_temp++;                                                                                                    // increasing interval of the function

    batt_temp = (BATT_TEMP_CONST * (map(analogRead(TEMP_degC_MEAS_PIN), TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX)) / 1000); // mapping analog signals of battery temp data

    if (num_meas_batt_temp % NUM_TEMP_AVERAGE == 0)
    {
        for (int i = 0; i < NUM_TEMP_AVERAGE; i++)
        {
            batt_temp += meas_batt_temp[i];
        }
        batt_temp /= NUM_TEMP_AVERAGE; // batt_temp is the average of all the measurments in the meas_batt_temp[] array. Giving the total average battery temperature.
        num_meas_batt_temp = 0;
        batt_temp_avail = true; // Set to true after first batt_temp value is avail. Avoids acting on overtemp before the first average is computed.

        // Serial.print("batt_temp: ");
        // Serial.println(batt_temp);
    }

    if (batt_temp_avail == true)
    {
        if (batt_temp < TEMP_THRESHOLD) // if statement to check if battery temperature is over the max amount.
        {
            overtemp_state = false;
        }
        if (batt_temp > TEMP_THRESHOLD)
        {
            delay(DEBOUNCE_DELAY);

            if (map(analogRead(TEMP_degC_MEAS_PIN), TEMP_ADC_MIN, TEMP_ADC_MAX, TEMP_MIN, TEMP_MAX) > TEMP_THRESHOLD)
            {
                overtemp_state = true;
            }
        }
    }

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void updateLCD(float batt_temp, float cell_voltages[])
{
    // Serial.print("**************************************************************************");

    bool LCD_Update = false;
    float time1 = 0;
    float packVoltage = 0; // V not mV
    float packTemp = 0;    // degC

    packVoltage = (static_cast<float>(cell_voltages[0]) / 1000);
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

    for (int i = 0; i < CELL_COUNT; i++)
    {
        float temp_cell_voltage = 0;
        temp_cell_voltage = (static_cast<float>(cell_voltages[i + 1]) / 1000);
        Serial3.print(i + 1); // Printing cell numbers with increased value.
        Serial3.print(":");
        Serial3.print(temp_cell_voltage, 1); // cellVoltage from BMS in V?  shows one decimal place
        if ((i + 1) % 3 == 0)                // modular division of 3
        {
            Serial3.print("V");
        }
        else
        {
            Serial3.print("V ");
        }
    }
    LCD_Update = false;
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactOverCurrent()
{
    if (packOverCurrent_state)
    {
        if (num_overcurrent == 0)
        {
            RoveComm.write(RC_BMSBOARD_PACKOVERCURRENT_DATA_ID, RC_BMSBOARD_PACKOVERCURRENT_DATA_COUNT, (uint8_t)packOverCurrent_state);
            delay(100);
            digitalWrite(PACK_GATE_CTR_PIN, LOW);
            time_of_overcurrent = millis();
            notifyOverCurrent();
            num_overcurrent++;
        }

        else if (num_overcurrent == 1)
        {
            if (millis() >= (time_of_overcurrent + RESTART_DELAY))
            {
                digitalWrite(PACK_GATE_CTR_PIN, HIGH);
            }

            if (millis() >= (time_of_overcurrent + RESTART_DELAY + RECHECK_DELAY))
            {
                time_of_overcurrent = 0;
                packOverCurrent_state = false;
                num_overcurrent = 0;
            }
        }

        else
        {
            RoveComm.write(RC_BMSBOARD_PACKOVERCURRENT_DATA_ID, RC_BMSBOARD_PACKOVERCURRENT_DATA_COUNT, (uint8_t)packOverCurrent_state);

            digitalWrite(PACK_GATE_CTR_PIN, LOW);
            notifyOverCurrent();
            delay(100);
            digitalWrite(PACK_GATE_CTR_PIN, HIGH); // BMS suicide
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactUnderVoltage()
{
    for (uint8_t i = 0; i < CELL_COUNT; i++)
    {
        if (cell_undervoltage_state &= (1 << i))
        {
            error_report |= (1 << i);
        }
        else
        {
            error_report &= !(1 << i);
        }
    }

    if (pack_undervoltage_state)
    {
        RoveComm.write(RC_BMSBOARD_PACKUNDERVOLTAGE_DATA_ID, RC_BMSBOARD_PACKUNDERVOLTAGE_DATA_COUNT, (uint8_t)pack_undervoltage_state);
        digitalWrite(PACK_GATE_CTR_PIN, LOW); // BMS Suicide
        notifyUnderVoltage();
    }

    if (cell_undervoltage_state)
    {
        RoveComm.write(RC_BMSBOARD_CELLUNDERVOLTAGE_DATA_ID, RC_BMSBOARD_CELLUNDERVOLTAGE_DATA_COUNT, error_report);
        notifyUnderVoltage();

        if (cell_undervoltage_count > 1)
        {
            digitalWrite(PACK_GATE_CTR_PIN, LOW); // BMS Suicide
            notifyUnderVoltage();
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactOverTemp()
{
    if (overtemp_state)
    {
        RoveComm.write(RC_BMSBOARD_PACKSUPERHOT_DATA_ID, RC_BMSBOARD_PACKSUPERHOT_DATA_COUNT, (uint8_t)overtemp_state);
        if (!(fans_on))
        {
            fans_on = true;
            digitalWrite(FAN_CTR_PIN, HIGH); // only one fan control is used to control all 4 fans colletively.
        }
    }

    if (overtemp_state == false && fans_on == true)
    {
        fans_on = false;
        digitalWrite(FAN_CTR_PIN, LOW);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactForgottenLogicSwitch()
{
    if (forgotten_logic_switch == true)
    {
        if (num_out_voltage_loops == 1)
        {
            time_switch_forgotten = millis();
            time_switch_reminder = millis();
        }
        if (num_out_voltage_loops > 1)
        {
            if (millis() >= time_switch_reminder + LOGIC_SWITCH_REMINDER)
            {
                time_switch_reminder = millis();
                notifyLogicSwitch();
            }
            if (millis() >= time_switch_forgotten + IDLE_SHUTOFF_TIME)
            {
                digitalWrite(PACK_GATE_CTR_PIN, HIGH); // BMS Suicide
            }
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactEstopReleased()
{
    if (forgotten_logic_switch == false && estop_released_beep == false)
    {
        estop_released_beep = true;
        notifyEstopReleased();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void reactLowVoltage(float cell_voltage[CELL_COUNT])
{
    ////Serial.println("reactLowVoltage");
    if ((cell_voltage[0] > PACK_UNDERVOLTAGE) && (cell_voltage[0] <= PACK_LOWVOLTAGE) && (low_voltage_state == false)) // first instance of low voltage
    {
        low_voltage_state = true;
        notifyLowVoltage();
        time_of_low_voltage = millis();
        num_low_voltage_reminder = 1;
    }
    else if ((cell_voltage[0] > PACK_UNDERVOLTAGE) && (cell_voltage[0] <= PACK_LOWVOLTAGE) && (low_voltage_state == true)) // following instances of low voltage
    {
        if (millis() >= (time_of_low_voltage + (num_low_voltage_reminder * LOGIC_SWITCH_REMINDER)))
        {
            notifyLowVoltage();
            num_low_voltage_reminder++;
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setEstop(uint8_t data)
{
    if (data == 0)
    {
        digitalWrite(PACK_GATE_CTR_PIN, LOW);

        notifyEstop();
                                            // BMS Suicide
                                            // If BMS is not turned off here, the PACK_OUT_CTR_PIN would be low and there would be no way to get it high again without reseting BMS anyway.
    }
    else
    {
        digitalWrite(PACK_GATE_CTR_PIN, LOW);
        notifyReboot();
        delay(data * 1000); // Receiving delay in seconds so it needs to be converted to msec.

        digitalWrite(PACK_GATE_CTR_PIN, HIGH);
    }
    return;
}

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
}

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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyEstopReleased() // Buzzer sound: beep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(75);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
}

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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notifyOverCurrent() // Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep
{
    digitalWrite(BUZZER_CTR_PIN, HIGH);
    digitalWrite(SW_ERR_PIN, HIGH);
    delay(3000);
    digitalWrite(BUZZER_CTR_PIN, LOW);
    digitalWrite(SW_ERR_PIN, LOW);

    return;
}

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
}

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
}

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
    Serial3.write(0x50); // P
    Serial3.write(0x52); // R
    Serial3.write(0x4F); // O
    Serial3.write(0x4D); // M
    Serial3.write(0x45); // E
    Serial3.write(0x54); // T
    Serial3.write(0x48); // H
    Serial3.write(0x45); // E
    Serial3.write(0x55); // U
    Serial3.write(0x53); // S
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
    Serial3.write(0x50); // P
    Serial3.write(0x52); // R
    Serial3.write(0x4F); // O
    Serial3.write(0x4D); // M
    Serial3.write(0x45); // E
    Serial3.write(0x54); // T
    Serial3.write(0x48); // H
    Serial3.write(0x45); // E
    Serial3.write(0x55); // U
    Serial3.write(0x53); // S
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


void telemetry()
{
    RoveComm.write(RC_BMSBOARD_PACKI_MEAS_DATA_ID, RC_BMSBOARD_PACKI_MEAS_DATA_COUNT, main_current);
    delay(100);
    RoveComm.write(RC_BMSBOARD_PACKV_MEAS_DATA_ID, RC_BMSBOARD_PACKV_MEAS_DATA_COUNT, pack_out_voltage);
    delay(100);
    RoveComm.write(RC_BMSBOARD_TEMP_MEAS_DATA_ID, RC_BMSBOARD_TEMP_MEAS_DATA_COUNT, batt_temp);
    delay(100);
    RoveComm.write(RC_BMSBOARD_CELLV_MEAS_DATA_ID, RC_BMSBOARD_CELLV_MEAS_DATA_COUNT, cell_voltages);
    Telemetry.begin(telemetry, 150000);
}