//
//  speech_api.c
//  mocktest
//
//  Created by Jeremy Rand on 2016-10-17.
//  Copyright © 2016 Jeremy Rand. All rights reserved.
//


#include <stdio.h>

#include "speech_api.h"
#include "speech.h"


bool speechIsBusy(void)
{
    static uint16_t old_outptr = 0;
    if (old_outptr == 0) {
        printf("endptr=%x\n", endptr);
    }
    if (old_outptr != outptr) {
        old_outptr = outptr;
        printf("%x ", old_outptr);
    }
    //printf("busy=0x%x, outptr=0x%x, endptr=0x%x\n", speechBusy, outptr, endptr);
    return (speechBusy != 0);
}


bool speakMessage(uint8_t *data, uint16_t dataLen)
{
    if (speechIsBusy())
        return false;
    
    speechData = data;
    speechLen = dataLen;
    setupSpeech();
    
    return true;
}
