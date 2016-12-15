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
    return (speechBusy != 0);
}


bool speakMessage(uint8_t *data, uint16_t dataLen)
{
    if (speechIsBusy())
        return false;
    
    speechData = data;
    speechLen = dataLen + 1;
    setupSpeech();
    
    return true;
}
