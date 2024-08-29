#ifndef STARTUP_H
#define STARTUP_H

#include <Arduino.h>

extern const uint8_t outputPins[];
extern int8_t numChannels;

void startupBlink()
{
    for (int i = 0; i < numChannels; i++)
    {
        digitalWrite(outputPins[i], HIGH);
        delay(100);
        digitalWrite(outputPins[i], LOW);
    }
}

#endif