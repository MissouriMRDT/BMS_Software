#include "LCD.h"

#include <cstdint>
#include "PinAssignments.h"
#include <SoftwareSerial.h>

SoftwareSerial OpenLCD(LCD_RX, LCD_TX);

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
    
void LCD_update(float temp, float packVoltage, float cell_voltages[], float current) {
    // Clear LCD
    OpenLCD.write('|'); // Enter settings mode
    OpenLCD.write('-'); // Clear display

    // Display pack voltage
    OpenLCD.printf("Pack:%.1f", packVoltage);
    OpenLCD.print("V ");

    // Display temp
    OpenLCD.printf("Tmp:%.1f", temp);
    OpenLCD.print("C");

    // Display cell voltages on LCD
    for (uint8_t i = 0; i < 6; i++)
    {
        if (i != 2 && i != 5)
        {
            OpenLCD.print(i + 1);
            OpenLCD.printf(":%.1f", cell_voltages[i]);
            OpenLCD.print("V ");
        }
        else
        {
            OpenLCD.print(i + 1);
            OpenLCD.printf(":%.1f", cell_voltages[i]);
            OpenLCD.print("V");
        }
    }

    // Display current
    OpenLCD.printf("Current:%.1f", current);
    OpenLCD.printf("A");
}
