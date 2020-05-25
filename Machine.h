// Copyright Maurice Montag 2020
// All Rights Reserved
// See LICENSE file for more information

#include<stdint.h>

#ifndef MACHINE_H  // header guards
#define MACHINE_H

// initializes the machine.
// takes a pointer to an array of program data, and an integer which represents the length of that data
// the data at the pointer is not changed in any way by this function
void createMachine(uint8_t* data, uint16_t length);

// performs a single CPU cycle/instruction in the CPU
void step(void);

// get the machine state of our Touring Machine
uint8_t getMachineState(void);
#endif
