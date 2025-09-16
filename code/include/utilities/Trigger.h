#ifndef TRIGGER_UTILS_H
#define TRIGGER_UTILS_H

#include <Arduino.h>

extern const uint8_t outputPins[];
extern int8_t numChannels;

class TriggerUtils {
  public:
    static void setAllOutputsLow() {
        for (int i = 0; i < numChannels; i++) {
            digitalWrite(outputPins[i], LOW);
        }
    }

    static void setOutput(uint8_t channel, bool state) {
        if (channel < numChannels) {
            digitalWrite(outputPins[channel], state ? HIGH : LOW);
        }
    }

    static void handleStepOverflow(uint8_t &stepCount, uint8_t maxSteps) {
        if (stepCount >= maxSteps) {
            stepCount = 0;
        }
    }

    static void applyDelay(int8_t msDelay) {
        if (msDelay > 0) {
            delay(msDelay);
        } else if (msDelay < 0) {
            delay(msDelay * -1);
        }
    }
};

#endif
