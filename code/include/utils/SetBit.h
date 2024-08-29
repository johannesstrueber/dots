#ifndef SET_BIT_H
#define SET_BIT_H

#include <Arduino.h>

extern const uint8_t seqMatrixSize;
extern byte seqMatrix[];
extern const uint8_t pages;
extern const uint8_t rows;
extern const uint8_t cols;

void setBit(int page, int row, int col)
{
    int index = page * rows * cols + row * cols + col;
    int byteIndex = index / 8;
    int bitIndex = index % 8;
    seqMatrix[byteIndex] |= (1 << bitIndex);
}

#endif