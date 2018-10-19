/*
 * main.c
 * mocktest
 *
 * Created by Jeremy Rand on 2016-09-10.
 * Copyright (c) 2016 Jeremy Rand. All rights reserved.
 *
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <mouse.h>

#include "mockingboard.h"
#include "drivers/a2_mouse_drv.h"


tMockingSoundRegisters soundData1 = {
    { TONE_PERIOD_A(4), 0, 0 },                         // Tone period for the three channels
    MIN_NOISE_PERIOD,                                   // Noise period
    ENABLE_CHANNEL(TONE_CHANNEL_A),                     // Enable
    { MAX_AMPLITUDE, MIN_AMPLITUDE, MIN_AMPLITUDE },    // Amplitude for the three channels
    MIN_ENVELOPE_PERIOD,                                // Envelope period
    ENVELOPE_SHAPE_ONE_SHOT_DECAY,                      // Envelope shape
    0,                                                  // Dummy1
    0                                                   // Dummy2
};


tMockingSoundRegisters soundData2 = {
    { TONE_PERIOD_F_SHARP(3), 0, 0 },                   // Tone period for the three channels
    MIN_NOISE_PERIOD,                                   // Noise period
    ENABLE_CHANNEL(TONE_CHANNEL_A),                     // Enable
    { MAX_AMPLITUDE, MIN_AMPLITUDE, MIN_AMPLITUDE },    // Amplitude for the three channels
    MIN_ENVELOPE_PERIOD,                                // Envelope period
    ENVELOPE_SHAPE_ONE_SHOT_DECAY,                      // Envelope shape
    0,                                                  // Dummy1
    0                                                   // Dummy2
};


uint8_t mySpeechData[] = {
    0xE8, 0x70, 0xA8, 0x5F, 0x00, 0xE8, 0x70, 0xA8,
    0x5F, 0x00, 0xE8, 0x70, 0xA8, 0x5F, 0x00, 0xE8,
    0x7B, 0xA8, 0x5F, 0x63, 0xE7, 0x6B, 0xA8, 0x59,
    0x47, 0xE8, 0x6A, 0xA8, 0x61, 0x36, 0xE8, 0x79,
    0xA8, 0x65, 0x37, 0xE7, 0x79, 0xA8, 0x69, 0x0E,
    0xE7, 0x6A, 0xA8, 0x61, 0x29, 0xE8, 0x6B, 0xA8,
    0x59, 0xAD, 0xE8, 0x6B, 0xA8, 0x51, 0xC0, 0xE8,
    0x6A, 0xA8, 0x51, 0x07, 0xE8, 0x6A, 0xA8, 0x4B,
    0x39, 0xE8, 0x6A, 0xA8, 0x49, 0x64, 0xE7, 0x6A,
    0x88, 0x49, 0x11, 0xE7, 0x5A, 0xB8, 0x51, 0x63,
    0xE7, 0x5A, 0xB8, 0x59, 0x1D, 0xE8, 0x7A, 0xA8,
    0x61, 0x65, 0xE8, 0x79, 0xA8, 0x61, 0xC0, 0xE8,
    0x70, 0x78, 0x51, 0x00, 0xE8, 0x70, 0x78, 0x51,
    0x00, 0xE8, 0x70, 0x78, 0x59, 0x00, 0xE7, 0x79,
    0xA8, 0x65, 0x04, 0xE7, 0x6A, 0x98, 0x61, 0x16,
    0xE8, 0x6B, 0xA8, 0x61, 0x60, 0xE8, 0x7C, 0xA8,
    0x59, 0x78, 0xE7, 0x7C, 0xA8, 0x55, 0x0A, 0xE8,
    0x6B, 0xA8, 0x62, 0x33, 0xE8, 0x6A, 0xA8, 0x59,
    0x1C, 0xE8, 0x7A, 0xA8, 0x51, 0x64, 0xE7, 0x7B,
    0x88, 0x51, 0x01, 0xE8, 0x7C, 0xA8, 0x59, 0x30,
    0xE8, 0x7D, 0xA8, 0x59, 0x27, 0xE7, 0x7D, 0x78,
    0x61, 0x01, 0xE8, 0x6C, 0xA8, 0x61, 0x28, 0xE8,
    0x6B, 0xA8, 0x59, 0x32, 0xE8, 0x6A, 0xA8, 0x4D,
    0x60, 0xE7, 0x29, 0xA8, 0x41, 0x0A, 0xE8, 0x78,
    0xA8, 0x41, 0x30, 0xE8, 0x70, 0xA8, 0x39, 0xFF,
    0xE8, 0x70, 0xA8, 0x39, 0x00
};


void delay(void)
{
    int i;
    for (i = 0; i < 1000; i++) ;
}


int main(void)
{
    mockingBoardInit(4, false);
    
    printf("HELLO, WORLD!\n");
    
    while (!kbhit()) {
        mockingBoardPlaySound(SPEAKER_LEFT, &soundData1);
        delay();
        mockingBoardStopSound(SPEAKER_LEFT);
        
        mockingBoardPlaySound(SPEAKER_RIGHT, &soundData2);
        delay();
        mockingBoardStopSound(SPEAKER_RIGHT);
    }
    
    cgetc();
    printf("RUN SPEECH TEST (Y/N) ");
    if (cgetc() == 'Y') {
        bool mouseInstalled = false;
        
        printf("\n");
        
        mockingBoardInit(4, true);
        
        if (mouse_install(&mouse_def_callbacks, &a2_mouse_drv) == 0) {
            mouseInstalled = true;
        }
        
        mockingBoardSpeak(mySpeechData, sizeof(mySpeechData));
        while (mockingBoardSpeechIsBusy()) {
            if (mouseInstalled) {
                struct mouse_info mouseInfo;
                
                mouse_info(&mouseInfo);
                printf("X:%3d Y:%3d %s\n", mouseInfo.pos.x, mouseInfo.pos.y,
                       ((mouseInfo.buttons != 0) ? "PRESSED" : ""));
            }
        }
        
        printf("\nDone speaking\n");
        cgetc();
        
        if (mouseInstalled)
            mouse_uninstall();
    }
    
    mockingBoardShutdown();
    
    return 0;
}
