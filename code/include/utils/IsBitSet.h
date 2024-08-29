#ifndef IS_BIT_SET_H
#define IS_BIT_SET_H

#include <Arduino.h>

extern const uint8_t seqMatrixSize;
extern byte seqMatrix[];
extern const uint8_t pages;
extern const uint8_t rows;
extern const uint8_t cols;

bool isBitSet(int page, int row, int col)
{
    int index = page * rows * cols + row * cols + col;
    int byteIndex = index / 8;
    int bitIndex = index % 8;
    return (seqMatrix[byteIndex] >> bitIndex) & 0x01;
}

#endif