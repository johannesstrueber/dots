#ifndef HANDLE_RESET_H
#define HANDLE_RESET_H

#include <Arduino.h>

enum ResetModeOptions
{
    RESET,
    NONE,
    STEP_RANDOM,
    STEP_BACK,
    SEQ_BAR_RANDOM,
    SEQ_PAGE_RANDOM
};

extern bool oldResetIn;
extern bool resetIn;
extern bool isPause;
extern uint8_t resetMode;
extern uint8_t seqCurrentPage;
extern uint8_t previousPage;
extern uint8_t stepCount;
extern const uint8_t cols;
extern uint8_t seqCurrentLength;
extern uint8_t seqCurrentOffset;
extern uint8_t page;

void handleReset()
{

    if (page != 1 && resetMode > 3)
        resetMode = 0;
    if (!oldResetIn && resetIn && !isPause)
        switch (resetMode)
        {
        case RESET:
            seqCurrentPage = 1;
            previousPage = 0;
            stepCount = 0;
            break;
        case NONE:
            break;
        case STEP_RANDOM:
            stepCount = random(0, cols);
            break;
        case STEP_BACK:
            if (stepCount == 0)
            {
                seqCurrentPage = previousPage;
                stepCount = cols - 1;
            }
            else
                stepCount--;
            break;
        case SEQ_BAR_RANDOM:
            stepCount = random(0, 3) * 4;
            break;
        case SEQ_PAGE_RANDOM:
            seqCurrentPage = random(1, seqCurrentLength + seqCurrentOffset + 1);
            previousPage = 0;
            stepCount = 0;
            break;
        default:
            break;
        }
}

#endif