#include "utilities/ResetHandler.h"

#include "State.h"
#include <Arduino.h>

void handleReset() {
    if (page != SEQUENCER && resetMode > STEP_BACK)
        resetMode = RESET;
    if (!oldResetIn && resetIn && !isPause)
        switch (resetMode) {
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
                if (stepCount == 0) {
                    seqCurrentPage = previousPage;
                    stepCount = cols - 1;
                } else
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
