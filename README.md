# BmsBoardSoftware
This is the software for Gryphon's BMS. As of Apr 02, 2017, the following functionality is implemented:

-Communication with and cell voltage readings from LTC6803

-Measurement of pack current and voltage

-Communication with power board and ultimately base station

-Overcurrent shutoff

-Manual fan and pack power control via base station

-Single-drop communication with and temperature readings from the DS18B20 one-wire temperature sensor

TODO:

-Transmission of voltages to indicator board

-Fuel gauge interaction

-Fix ADC scaling for LTC readings and/or find causes of noise

-Verify/improve accuracy of temperature sensing

-Idle shutoff and Estop alarm   



There are a few linker options with hardcoded paths to files in the current project -- it is recommended to create your own, import the source files, and solve the linker and compiler search path issues in your own way.
