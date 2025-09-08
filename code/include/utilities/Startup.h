#ifndef STARTUP_H
#define STARTUP_H

#include "Trigger.h"
#include <Arduino.h>

extern const uint8_t outputPins[];
extern int8_t numChannels;

void startupBlink() {
    for (int i = 0; i < numChannels; i++) {
        TriggerUtils::setOutput(i, true);
        delay(100);
        TriggerUtils::setOutput(i, false);
    }
}

#endif