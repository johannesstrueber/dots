#include "modes/StepSequencerMode.h"

#include "State.h"
#include "UiStrings.h"
#include "utilities/BitUtils.h"
#include "utilities/DisplayUtils.h"
#include "utilities/EncoderUtils.h"
#include "utilities/MenuLayout.h"
#include "utilities/TriggerUtils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <EEPROM.h>

namespace {

constexpr uint8_t RECT_WIDTH = 8;
constexpr uint8_t RECT_HEIGHT = 7;

enum SeqEncoderOptions { SEQ_ENC_PAGE = 96, SEQ_ENC_LENGTH, SEQ_ENC_OFFSET, SEQ_ENC_PLAY, SEQ_ENC_RESET, SEQ_ENC_BACK };

uint8_t markerRow = 0;
uint8_t markerCol = 0;

void toggleMatrixCell(int page, int row, int col) {
    if (BitUtils::isBitSet(page, row, col))
        BitUtils::clearBit(page, row, col);
    else
        BitUtils::setBit(page, row, col);
}

} // namespace

void saveStepSequencerSettings() {
    // Save step patterns
    for (int pa = 0; pa < pages; pa++) {
        for (int ro = 0; ro < rows; ro++) {
            for (int co = 0; co < cols; co++) {
                int index = pa * rows * cols + ro * cols + co;
                int safeByteIndex = (index - (index % 8)) / 8;
                if (safeByteIndex < seqMatrixSize) {
                    if (BitUtils::isBitSet(pa, ro, co)) {
                        byte currentByte = EEPROM.read(safeByteIndex);
                        EEPROM.write(safeByteIndex, currentByte | (1 << (index % 8)));
                    } else {
                        byte currentByte = EEPROM.read(safeByteIndex);
                        EEPROM.write(safeByteIndex, currentByte & ~(1 << (index % 8)));
                    }
                }
            }
        }
    }

    // Save sequencer parameters
    EEPROM.write(384, seqCurrentPage);
    EEPROM.write(385, seqCurrentLength);
    EEPROM.write(386, seqCurrentOffset);
}

void loadStepSequencerSettings() {
    // Load step patterns
    for (int pa = 0; pa < pages; pa++) {
        for (int ro = 0; ro < rows; ro++) {
            for (int co = 0; co < cols; co++) {
                int index = pa * rows * cols + ro * cols + co;
                int safeByteIndex = (index - (index % 8)) / 8;
                if (safeByteIndex < seqMatrixSize) {
                    if (EEPROM.read(safeByteIndex) & (1 << (index % 8))) {
                        BitUtils::setBit(pa, ro, co);
                    } else {
                        BitUtils::clearBit(pa, ro, co);
                    }
                }
            }
        }
    }

    // Load sequencer parameters
    seqCurrentPage = EEPROM.read(384);
    seqCurrentLength = EEPROM.read(385);
    seqCurrentOffset = EEPROM.read(386);

    // Validate ranges
    if (seqCurrentPage > pages)
        seqCurrentPage = 1;
    if (seqCurrentLength > pages)
        seqCurrentLength = 1;
    if (seqCurrentOffset > pages)
        seqCurrentOffset = 0;
}

void renderStepSequencerScreen() {
    static char seqBuffer[6];

    display.setCursor(0, 0);

    if (enc > 0 || enc < SEQ_ENC_PAGE || previousPage != seqCurrentPage) {
        display.clearDisplay();
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                bool isActive = BitUtils::isBitSet(seqCurrentPage - 1, row, col);
                uint8_t x = col * 8;
                uint8_t y = row * 7;

                if (isActive) {
                    display.drawFastVLine(x + RECT_WIDTH, y + 2, RECT_HEIGHT - 4, WHITE);
                    display.drawFastHLine(x + 3, y + RECT_HEIGHT, RECT_WIDTH - 5, WHITE);
                } else
                    display.fillRoundRect(x, y, RECT_WIDTH + 1, RECT_HEIGHT, 5, WHITE);
            }
        }
        if (enc < SEQ_ENC_PAGE) {
            uint8_t markerX = markerCol * 8;
            uint8_t markerY = markerRow * 7;

            bool color = BitUtils::isBitSet(seqCurrentPage - 1, markerRow, markerCol);

            display.fillRoundRect(markerX + 1, markerY + 1, RECT_WIDTH - 1, RECT_HEIGHT - 1, 5, color ? BLACK : WHITE);
            display.fillRoundRect(markerX + 2, markerY + 2, RECT_WIDTH - 3, RECT_HEIGHT - 3, 3, color ? WHITE : BLACK);
        }
    }

    display.drawFastVLine(0, 0, 41, BLACK);
    display.fillRect(0, 42, 128, 3, BLACK);
    for (int i = 1; i * barLength < cols; i++) {
        uint8_t xPosition = i * barLength * 8;
        display.fillRect(xPosition, 43, 1, 2, WHITE);
    }
    display.fillRect((stepCount + 1) * 8 - 6, 43, 4, 2, WHITE);

    DisplayUtils::drawPages(0, MenuLayout::MENU_Y_TOP, seqCurrentPage, pages, enc == SEQ_ENC_PAGE, seqBuffer);
    DisplayUtils::drawNumberText(32, MenuLayout::MENU_Y_TOP, seqCurrentLength, lengthText, enc == SEQ_ENC_LENGTH, seqBuffer);
    DisplayUtils::drawNumberText(64, MenuLayout::MENU_Y_TOP, seqCurrentOffset, offsetText, enc == SEQ_ENC_OFFSET, seqBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_TOP, isPause ? pauseText : playText, enc == SEQ_ENC_PLAY, seqBuffer);

    DisplayUtils::drawDelay(0, MenuLayout::MENU_Y_BOTTOM, msDelay, seqBuffer);
    DisplayUtils::drawBPM(32, MenuLayout::MENU_Y_BOTTOM, bpm, intClock, clkMode, seqBuffer);
    DisplayUtils::drawMenuItemFromArray(64, MenuLayout::MENU_Y_BOTTOM, resetOptions, resetMode, enc == SEQ_ENC_RESET, seqBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_BOTTOM, backText, enc == SEQ_ENC_BACK, seqBuffer);

    display.display();
}

void updateStepSequencer() {
    updateScreen = false;

    TriggerUtils::applyDelay(msDelay);

    EncoderUtils::handleEncoderBounds(enc, 0, SEQ_ENC_BACK);

    if (enc < cols * rows) {
        markerCol = enc % cols;
        markerRow = enc / cols;
    }

    if (buttonOn)
        switch (enc) {
            case SEQ_ENC_PAGE:
                previousPage = seqCurrentPage;
                seqCurrentPage = seqCurrentPage >= pages ? 1 : seqCurrentPage + 1;
                break;
            case SEQ_ENC_LENGTH:
                seqCurrentLength++;
                if (seqCurrentLength > pages) {
                    seqCurrentLength = 1;
                }
                if (seqCurrentLength + seqCurrentOffset > pages) {
                    seqCurrentOffset = pages - seqCurrentLength;
                }
                break;
            case SEQ_ENC_OFFSET:
                seqCurrentOffset++;
                if (seqCurrentOffset >= pages) {
                    seqCurrentOffset = 0;
                }
                if (seqCurrentLength + seqCurrentOffset > pages) {
                    seqCurrentLength = pages - seqCurrentOffset;
                }
                break;
            case SEQ_ENC_RESET:
                if (resetMode == SEQ_PAGE_RANDOM)
                    resetMode = RESET;
                else
                    resetMode++;
                break;
            case SEQ_ENC_PLAY:
                EncoderUtils::toggleParameter(isPause);
                break;
            case SEQ_ENC_BACK:
                page = MAIN_MENU;
                updateScreen = true;
                break;
            default:
                toggleMatrixCell(seqCurrentPage - 1, markerRow, markerCol);
                break;
        }

    TriggerUtils::handleStepOverflow(stepCount, cols);

    if (stepCount == 0 && !isPause && updateTrigger) {
        if (seqCurrentPage <= pages) {
            if (seqCurrentOffset + seqCurrentLength <= seqCurrentPage)
                seqCurrentPage = seqCurrentOffset + 1;
            else
                seqCurrentPage++;
        } else
            seqCurrentPage = seqCurrentOffset + 1;
        previousPage = seqCurrentPage;
        stepCount = 0;
    }

    if (!isPause && updateTrigger) {
        for (int i = 0; i < numChannels; i++) {
            bool shouldTrigger = !BitUtils::isBitSet(seqCurrentPage - 1, i, stepCount);
            TriggerUtils::setOutput(i, shouldTrigger);
        }
        updateTrigger = false;
    }

    renderStepSequencerScreen();

    TriggerUtils::applyDelay(msDelay);
}
