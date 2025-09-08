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

    static uint8_t countActiveSteps(uint32_t pattern, uint8_t steps) {
        uint8_t count = 0;
        for (int i = 0; i < steps; i++) {
            if (isStepActive(pattern, i))
                count++;
        }
        return count;
    }

    static uint32_t generateRandomPattern(uint8_t steps, uint8_t density) {
        uint32_t pattern = 0;
        for (int i = 0; i < steps; i++) {
            if (random(0, 12) < density) {
                pattern |= (1UL << i);
            }
        }
        return pattern;
    }

    static void calculateFibonacci(int n, int *fibonacciSequence) {
        if (n > 0)
            fibonacciSequence[0] = 1;
        if (n > 1)
            fibonacciSequence[1] = 1;
        for (int i = 2; i < n; i++) {
            fibonacciSequence[i] = fibonacciSequence[i - 1] + fibonacciSequence[i - 2];
        }
    }

    static bool isPrime(int num) {
        if (num <= 1)
            return false;
        for (int i = 2; i <= sqrt(num); i++) {
            if (num % i == 0)
                return false;
        }
        return true;
    }

    static int mapValue(int value, int fromMin, int fromMax, int toMin, int toMax) {
        return map(value, fromMin, fromMax, toMin, toMax);
    }
};

#endif
