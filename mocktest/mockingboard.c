//
//  mockingboard.c
//  mocktest
//
//  Created by Jeremy Rand on 2016-09-10.
//  Copyright © 2016 Jeremy Rand. All rights reserved.
//


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "mockingboard.h"


// Defines

#define LATCH_COMMAND 0x7
#define WRITE_COMMAND 0x6
#define RESET_COMMAND 0x0
#define THROUGH_PORT_B 0x4


// Globals

// Addresses for the two 6522's (assuming slot 4 for now)
static uint8_t *gMockPortB[NUM_SOUND_CHIPS]    = { (uint8_t *)0xc400, (uint8_t *)0xc480 };
static uint8_t *gMockPortA[NUM_SOUND_CHIPS]    = { (uint8_t *)0xc401, (uint8_t *)0xc481 };
static uint8_t *gMockDataDirB[NUM_SOUND_CHIPS] = { (uint8_t *)0xc402, (uint8_t *)0xc482 };
static uint8_t *gMockDataDirA[NUM_SOUND_CHIPS] = { (uint8_t *)0xc403, (uint8_t *)0xc483 };

static uint8_t gMockingBoardInitialized = false;


static uint8_t *mapIOPointer(uint8_t slot, uint8_t *ptr)
{
    uint16_t temp1 = (uint16_t)ptr;
    uint16_t temp2 = slot;
    
    temp2 << 8;
    temp1 &= 0xf0ff;
    
    temp1 |= temp2;
    ptr = (uint8_t *)temp1;
    return ptr;
}

void mockingBoardInit(tSlot slot)
{
    tSoundChip soundChip;
    
    if (sizeof(tMockingSoundRegisters) != 16) {
        printf("The sound registers must be 16 bytes long!\n");
    }
    
    for (soundChip = SOUND_CHIP_1; soundChip < NUM_SOUND_CHIPS; soundChip++) {
#if 0
        gMockPortB[soundChip] = mapIOPointer(slot, gMockPortB[soundChip]);
        gMockPortA[soundChip] = mapIOPointer(slot, gMockPortA[soundChip]);
        gMockDataDirB[soundChip] = mapIOPointer(slot, gMockDataDirB[soundChip]);
        gMockDataDirA[soundChip] = mapIOPointer(slot, gMockDataDirA[soundChip]);
#endif
        
        *(gMockDataDirA[soundChip]) = 0xff;     // Set port A for output
        *(gMockDataDirB[soundChip]) = 0x7;      // Set port B for output
    }
    
    gMockingBoardInitialized = true;
}


static void writeCommand(tSoundChip soundChip, uint8_t command)
{
    volatile uint8_t *ptr = gMockPortB[soundChip];
    
    *ptr = command;
    *ptr = THROUGH_PORT_B;
}


void mockingBoardLatch(tSoundChip soundChip)
{
    writeCommand(soundChip, LATCH_COMMAND);
}


void mockingBoardWrite(tSoundChip soundChip)
{
    writeCommand(soundChip, WRITE_COMMAND);
}


void mockingBoardReset(tSoundChip soundChip)
{
    writeCommand(soundChip, RESET_COMMAND);
}


void mockingBoardTableAccess(tSoundChip soundChip, tMockingSoundRegisters *registers)
{
    uint8_t *data = (uint8_t *)registers;
    volatile uint8_t *ptr = gMockPortA[soundChip];
    uint8_t index;
    
    mockingBoardReset(soundChip);
    for (index = 0; index < 16; index++) {
        *ptr = index;
        mockingBoardLatch(soundChip);
        *ptr = *data;
        mockingBoardWrite(soundChip);
        data++;
    }
}
