#include "utilities/Startup.h"

#include "State.h"
#include "utilities/TriggerUtils.h"
#include <Arduino.h>

void startupBlink() {
    for (int i = 0; i < numChannels; i++) {
        TriggerUtils::setOutput(i, true);
        delay(100);
        TriggerUtils::setOutput(i, false);
    }
}
