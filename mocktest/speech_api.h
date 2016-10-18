//
//  speech_api.h
//  mocktest
//
//  Created by Jeremy Rand on 2016-10-17.
//  Copyright © 2016 Jeremy Rand. All rights reserved.
//

#ifndef __mocktest__speech_api__
#define __mocktest__speech_api__


#include <stdbool.h>
#include <stdint.h>


extern bool speechIsBusy(void);
extern bool speakMessage(uint8_t *data, uint16_t dataLen);



#endif /* defined(__mocktest__speech_api__) */
