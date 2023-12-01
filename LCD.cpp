
void LCD_init() {
    OpenLCD.begin(9600); // Start communication with LCD over serial
    // OpenLCD.write('|');       // Put LCD in setting mode
    // OpenLCD.write(18);        // Set baud rate to 1000000
    // OpenLCD.end();
    // OpenLCD.begin(115200);

    OpenLCD.write('|');
    OpenLCD.write(24); // Send contrast command
    OpenLCD.write(1);  // Set contrast

    OpenLCD.write('|');
    OpenLCD.write(128 + 0); // Set white/red backlight amount to 0% (+29 is 100%)

    OpenLCD.write('|');
    OpenLCD.write(158 + 0); // Set green backlight amount to 0% (+29 is 100%)

    OpenLCD.write('|');
    OpenLCD.write(188 + 0); // Set blue backlight amount to 0% (+29 is 100%)
}
    
void LCD_update(/*pass vars*/) {
    // Clear LCD
    OpenLCD.write('|'); // Enter settings mode
    OpenLCD.write('-'); // Clear display

    // Display pack voltage
    OpenLCD.printf("Pack:%.1f", pack_out_voltage / 1000);
    OpenLCD.print("V ");

    // Display temp
    float batt_temp_F = ((batt_temp / 1000.0f) * (9.0f / 5.0f)) + 32.0f;

    OpenLCD.printf("Tmp:%.1f", ((batt_temp / 1000.0f) * (9.0f / 5.0f)) + 32.0f);
    OpenLCD.print("F");

    // Display cell voltages on LCD
    for (uint8_t i = 0; i < CELL_COUNT; i++)
    {
        if (i != 2 && i != 5)
        {
            OpenLCD.print(i + 1);
            OpenLCD.printf(":%.1f", cell_voltages[i] / 1000);
            OpenLCD.print("V ");
        }
        else
        {
            OpenLCD.print(i + 1);
            OpenLCD.printf(":%.1f", cell_voltages[i] / 1000);
            OpenLCD.print("V");
        }
    }
}
