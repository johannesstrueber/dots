#ifndef PATTERN_MATH_H
#define PATTERN_MATH_H

#include <Arduino.h>

class PatternMath {
  public:
    //  Bjorklund's algorithm
    static uint32_t generateEuclideanPattern(uint8_t steps, uint8_t hits) {
        if (hits == 0 || steps == 0 || hits > steps)
            return 0;

        uint32_t pattern = 0;
        int bucket = 0;

        for (int i = 0; i < steps; i++) {
            bucket += hits;
            if (bucket >= steps) {
                bucket -= steps;
                pattern |= (1UL << i);
            }
        }

        return pattern;
    }

    static uint32_t rotatePattern(uint32_t pattern, uint8_t steps, uint8_t rotation) {
        if (rotation == 0 || steps == 0)
            return pattern;

        rotation = rotation % steps;
        uint32_t mask = (1UL << steps) - 1;

        return ((pattern >> rotation) | (pattern << (steps - rotation))) & mask;
    }

    static bool isStepActive(uint32_t pattern, uint8_t step) {
        return (pattern >> step) & 1;
    }
};

#endif
