#ifndef LCD_H
#define LCD_H

#include "PinAssignments.h"
#include <SoftwareSerial.h>

SoftwareSerial OpenLCD(LCD_RX, LCD_TX);

void LCD_init();
void LCD_update(float temp, float packVoltage, float cell_voltages[]);

#endif // LCD_H