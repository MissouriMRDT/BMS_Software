#ifndef SYSINIT_TIVATM4C1294NCPDT_H_
#define SYSINIT_TIVATM4C1294NCPDT_H_

//initializes the system; turns on main cpu clock, system tick clock,
//and turns on all the basic hardware peripherals such as gpio pins.
//The RoveBoard file probably ensures that this function is ran before main.cpp
void initSystem();

#endif
