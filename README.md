# BmsBoardSoftware
This software is, clearly, meant to be run the Batter Management System. It is primarily based around protecting the battery pack, cutting the connection to the Power Board when conditions are dangerous or harmful. The drivers for the LTC6803 were taken from Solar Car BMS code.
As of the time of writing, (11/18/16) the following features are implemented:
    Switch the pack on and off based on current (high limit) and voltage (low limit), and command from Power Board
    Update LED fual gauge values
    Switches off logic power while idle for a long tme (~1 hr)
    Reports pack voltage and current on command from Power Board
To be implemented, in order of importance:
    Read voltages from LTC6803, then report back to PB and/or shut off pack 
    Temperature reading
    Fan Control
    Fix values for fuel gauge
