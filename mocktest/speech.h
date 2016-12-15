//
//  speech.h
//  mocktest
//
//  Created by Jeremy Rand on 2016-10-17.
//  Copyright © 2016 Jeremy Rand. All rights reserved.
//

#ifndef speech_h
#define speech_h


extern uint8_t *speechData;
extern uint16_t speechLen;
extern uint8_t speechBusy;

extern void setupSpeech(void);
extern void unsetupSpeech(void);


#endif /* speech_h */
