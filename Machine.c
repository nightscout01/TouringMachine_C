// Copyright Maurice Montag 2020
// All Rights Reserved
// See LICENSE file for more information

#include <stdio.h>
#include <stdbool.h> // so we can use boolean types
#include <stdlib.h>
#include <stdint.h>
#include "Machine.h"

typedef enum addressingMode{  // an enum for the architecture's three different addressing modes
    Immediate,
    Relative,
    Absolute,
} addressingMode_t;

// Function prototypes for our local functions
static void decodeInstruction(uint8_t inst);
static void erase(addressingMode_t addressingMode);
static void jump(addressingMode_t addressingMode);
static void load(addressingMode_t addressingMode);
static void store(addressingMode_t addressingMode);
static void incrementState(void);
static void decrementState(void);
static void compare(addressingMode_t addressingMode);
static void branchIfEqual(addressingMode_t addressingMode);
static void branchIfNotEqual(addressingMode_t addressingMode);
static void halt(void);

static uint16_t getMemoryAddress(addressingMode_t addressingMode);
static void branchHelper(int8_t branch);


const bool DEBUG = true;  // if enabled, causes debug information to be printed out

uint8_t* memory;  // our array of memory for our machine. 16 bit memory bus.
uint16_t PC;  // our program counter for our emulator
uint8_t machineState;  // our single register or machine state
uint8_t flags;         // our status flag byte, used for storing data about comparisons, among other things
bool running;  // used for the halt signal. When set to false, requires another call to createMacbine
    // to continue running

// Passes in the program to execute into our memory
// Takes a pointer to the data to pass into our memory and the length in bytes of that data
// the length of our program cannot be larger than the length of our addressable memory
void createMachine(uint8_t* data, uint16_t length){
    memory = (uint8_t*) malloc(sizeof(uint8_t) * 65536); // init our 64 KiB memory array
    for(int i = 0; i < length; i++){
        memory[512+i] = data[i];  // programs start at memory address 0x200 (512).
    }
    PC = 512;   // set the PC to the beginning of our program
    flags = 0;  // flags start empty
    machineState = 0;  // machine state starts empty too
    running = true;
}

// Performs a single CPU step
void step(void){
    if(running){
        decodeInstruction(memory[PC]);
    }
}

static void decodeInstruction(uint8_t inst){
    switch (inst)
            {
                case 0x1:
                    // reserved
                    break;
                case 0x2:  // NOP
                    PC += 2;
                    break;
                case 0x3:
                    erase(Relative);
                    PC += 2;
                    break;
                case 0x4:
                    erase(Absolute);
                    PC += 3;
                    break;
                case 0x5:  // more like branch always
                    jump(Immediate);
                    break;
                case 0x6:
                    jump(Absolute);
                    break;
                case 0x7:  
                    load(Immediate);
                    PC += 2;
                    break;
                case 0x8:
                    load(Relative);
                    PC += 2;
                    break;
                case 0x9:
                    load(Absolute);
                    PC += 3;
                    break;
                case 0xA:
                    store(Relative);
                    PC += 2;
                    break;
                case 0xB:
                    store(Absolute);
                    PC += 3;
                    break;
                case 0xC:
                    incrementState();
                    PC += 1;
                    break;
                case 0xD:
                    decrementState();
                    PC += 1;
                    break;
                case 0xE:
                    compare(Immediate);
                    PC += 2;
                    break;
                case 0xF:
                    compare(Relative);
                    PC += 2;
                    break;
                case 0x10:
                    compare(Absolute);
                    PC += 2;
                    break;
                case 0x11:
                    branchIfEqual(Immediate);
                    break;
                case 0x12:
                    branchIfNotEqual(Immediate);
                    break;
                case 0xFF:
                    halt();
                    break;
            }
            if(DEBUG){
                printf("Current Instruction %#02x\n", inst);  // if debug is enabled, print out the current instruction
            }
}


static void halt(void){
    running = false;
}

static void store(addressingMode_t addressingMode){
    uint16_t idx = getMemoryAddress(addressingMode);
    memory[idx] = machineState;
}

static void erase(addressingMode_t addressingMode){
    uint16_t idx = getMemoryAddress(addressingMode);
    memory[idx] = 0;
}

static void jump(addressingMode_t addressingMode){
    PC = getMemoryAddress(addressingMode);
}

static void compare(addressingMode_t addressingMode){
    uint16_t idx = getMemoryAddress(addressingMode);
    if(memory[idx] == machineState){
        flags |= 0x1;  // set the LSB (equals flag) of the flag byte to 1
    } else {
        flags &= 0xFE;  // set the LSB (equals flag) of the flag byte to 0
    }
}

static void branchIfEqual(addressingMode_t addressingMode){
    uint8_t relativeBranch =  memory[getMemoryAddress(addressingMode)];
    if((flags & 0x1) == 1){   // if the equals flag is set
        int8_t branch = (int8_t)relativeBranch;
        branchHelper(branch); // we take the branch 
    } else {  // if it is not set, we do not take the branch
        PC+=2;
    }
}

static void branchIfNotEqual(addressingMode_t addressingMode){
    uint8_t relativeBranch =  memory[getMemoryAddress(addressingMode)];
    if((flags & 0x1) == 0){   // if the equals flag is set
        int8_t branch = (int8_t)relativeBranch;
        branchHelper(branch); // we take the branch 
    } else {  // if it is not set, we do not take the branch
        PC+=2;
    }
}

static void load(addressingMode_t addressingMode){
    uint16_t idx = getMemoryAddress(addressingMode);
    machineState = memory[idx];
}

static void incrementState(void){
    machineState++;
}

static void decrementState(void){
    machineState--;
}

static void branchHelper(int8_t branch){
    PC += branch;  // add the signed branch value to our program counter, 
    // moving it forwards or backwards to a max range of (-128 to +127)
}

uint8_t getMachineState(void){  // getter method for our machine state/register
    return machineState;
}

static uint16_t getMemoryAddress(addressingMode_t addressingMode){
    switch(addressingMode){
        case Immediate:
            return PC+1;
        case Relative:
            return PC+((int16_t)memory[PC+1]);  // return the relative offset from the current PC given in PC+1 as specified
        case Absolute:
            return memory[PC+1] | (memory[PC+2] << 8);
        default:
            return -1;  // error
    }
}
