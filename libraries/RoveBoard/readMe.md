# RoveBoard! (for all places that aren't energia)

## Overview
Programmers: 
Primary structure: Drue Satterfield 

Individual files: Varied

This is rover's official Hardware Abstraction Layer (HAL), a series of functions designed to a) be able to be used on any microcontroller that rover uses (in theory, check which boards are supported and which types of functions they support further down) b) allow the programmer to work with the hardware on the microcontroller easily and accessibly with easy-to-read and understand function calls C) allow roveware files like RoveComm, which are designed to be used across rover, to be used on every microcontroller we're using by using the standardized functions provided in this framework, which are promised to work on every microcontroller that supports them.

It's fairly similar to arduino or energia, if you know what those are (directly inspired by, in fact) but has been generally expanded and unlike those two, the functions in roveboard are well documented and flat out tell you WHAT parts of the hardware the functions use, without you having to know HOW they're carrying out the function. In this way, you know what's going on in the microcontroller when you use roveboard, while still getting the easy hardware-accessing functions to use. Also unlike the former two, each microcontroller not only has standardized functions provided for roveware files to use on every microcontroller that supports the hardware feature they use, but also a series of functions that take specific advantage of the board for main.cpp and related files to use if they wish. The latter functions will only work with their own processor, but that's perfectly fine for non-portable files like main.cpp.

The framework is designed to be used by any IDE and c++ compiler as well (unless the compilers have different ideas of what constitutes acceptable c++), so you can take it with you wherever you go...except for energia, which needs and has its own specific version.

It's not necessary for every microcontroller to implement every feature noted in generics, but if they do then they must at least implement the standard functions for that feature.

Be sure to read the wiki for more helpful things such as visual pinmaps.

## Files and folders
* At the top, you'll see a series of files that follow the naming pattern of `RoveBoard.h` and `RoveBoard_(microcontroller name).h`. 
  - The former is the primary include for non-board-specific roveware files like RoveComm to include, and returns a list of prototyped functions for it to use. But, `Roveboard.h` importantly doesn't return the actual definitions of those functions, just the names and the fact that they exist. 
  - The other roveboard files like `RoveBoard_TM4C1294NCPDT.h` return those functions and others that work specifically on the tm4c and define them. You need to include the roveboard for your specific microcontroller in your main.cpp file for everything to work.

* The folders inside RoveBoard are split into three categories. 
  - `standardized_API`, which contain files that prototype the standardized functions for each feature, and the typenames and constants associated with the features.
  - `supportingUtilities`, which contain supporting functions, macros, and classes that aren't device-specific so are there to be helpful to the entire framework 
  - lastly you'll see folders devoted to a specific processor like 'tm4c1294ncpdt_API'. Those folders contain the files that list and define the functions made for their processor, and what most users of RoveBoard will be interested in looking at.

## Supported microcontrollers and what features they implement
* Tiva tm4c1294ncpdt - pwm writing, pwm reading, clocking, digital pin, Ethernet, Uart/serial, timerInterface, I2c, PermanentMemory(EEPROM version), ADC, Watchdog
* Msp432P4O1R - clocking, digital pin, Uart/serial, and nonstandard Power API.


## Dependencies
* Note that all external dependencies can be downloading via links in this repo's wiki section.
* The Msp432P4O1R uses the SMSIS framework, which itself requires you to compile using an IAR, KEIL, generic ARM, GCC, or TI-CCS compiler.

## Usage
1) In your main.cpp, before anything else include the roveboard_x.h file for the processor you're using
2) From a coding standpoint, that's it. Make sure to read the H files for your processor, as they contain not only the functions you're free to use in your files, but also any warnings, bugs or cautions.
3) If needed, tell the compiler where the RoveBoard folder is
4) If needed, tell the linker where the .lib file is (should be in the processor folder, for whatever processor you're using)
5) Make sure that whatever roveware you're using is supported by the microcontroller; for instance, RoveComm can only be used on boards that support RoveEthernet

99) If you're making a new roveware component, have it include "RoveBoard.h" to get all the standardized functions you can use.



## Adding more boards/functions to existing boards
1) Make a folder for it, and a RoveBoard_x.h for it. 
2) If your board supports one of the standard listed features under generics, make sure you implement at least the standard functions before you put it in.
3) You'll need to generate a .lib file for your new library, so that linkers can actually use it. Most psuedo-professional IDE's will have ways for you to generate them.
4) Make sure it compiles and test it first; you can use the included example code from 2016's arm, if you want
5) For pete's sake, make sure the H files at the very least include halfway decent comments/documentation; make sure at the least the programmers know what hardware components are used when a function is called so they can avoid conflicts if necessary and aren't blindsided by anything. High visibility is a key difference between what we do and what arduino/energia does
6) Be sure to update the pinmap and dependencies pages in the wiki, if relevant
