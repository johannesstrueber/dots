#ifndef ENCODER_UTILS_H
#define ENCODER_UTILS_H

#include <Arduino.h>

class EncoderUtils
{
public:
    static void handleEncoderBounds(int8_t &enc, int8_t minValue, int8_t maxValue)
    {
        if (enc > maxValue)
            enc = minValue;
        else if (enc < minValue)
            enc = maxValue;
    }

    static void toggleParameter(bool &parameter)
    {
        parameter = !parameter;
    }

    static void cycleEnum(uint8_t &enumValue, uint8_t maxValue)
    {
        enumValue = (enumValue >= maxValue) ? 0 : enumValue + 1;
    }
};

#endif
