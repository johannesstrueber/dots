
#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <Arduino.h>

extern const uint8_t seqMatrixSize;
extern byte seqMatrix[];
extern const uint8_t pages;
extern const uint8_t rows;
extern const uint8_t cols;

class BitUtils {
  public:
    static bool isBitSet(int page, int row, int col) {
        int index = page * rows * cols + row * cols + col;
        int byteIndex = index / 8;
        int bitIndex = index % 8;
        return (seqMatrix[byteIndex] >> bitIndex) & 0x01;
    }

    static void setBit(int page, int row, int col) {
        int index = page * rows * cols + row * cols + col;
        int byteIndex = index / 8;
        int bitIndex = index % 8;
        seqMatrix[byteIndex] |= (1 << bitIndex);
    }

    static void clearBit(int page, int row, int col) {
        int index = page * rows * cols + row * cols + col;
        int byteIndex = index / 8;
        int bitIndex = index % 8;
        seqMatrix[byteIndex] &= ~(1 << bitIndex);
    }
};

#endif
