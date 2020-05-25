#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Machine.h"

const char* filePath = "Adding.bin";
FILE *filePtr;
uint8_t *buf;
long filelen;

int main()
{
    // create some sort of timing logic to simulate regular clock cycles insteaf of just an
    // unbounded while loop

    filePtr = fopen(filePath, "rb");  // we read in program data from the provided filepath 
    fseek(filePtr, 0, SEEK_END);
    filelen = ftell(filePtr);
    rewind(filePtr);

    buf = (uint8_t*)malloc(filelen * sizeof(uint8_t));  // allocate enough memory for the bytes in our program file
    fread(buf, filelen, 1, filePtr);  // read the file contents into our buffer
    fclose(filePtr);  // close the file

    if(filelen > 65536){  // the user is trying to read more than 2^16 bytes into our RAM, there's not enough space
        return EXIT_FAILURE;
    }
    createMachine(buf, filelen);
    while(1){
        printf("Machine State: %d", getMachineState());  // print the current machine state
        getchar();  // wait for keypress (we're going to be single stepping through this)
        step();  // perform a CPU step/cycle
    }
}



