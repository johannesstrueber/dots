#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "utilities/Bit.h"
#include "utilities/Display.h"
#include "utilities/Encoder.h"
#include "utilities/MenuLayout.h"
#include "utilities/PatternMath.h"
#include "utilities/Trigger.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <Wire.h>

#define OLED_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define RECT_WIDTH 8
#define RECT_HEIGHT 7
#define RECT_SPACING_X 0
#define RECT_SPACING_Y 0

extern int8_t numChannels;
extern const uint8_t outputPins[];

extern const uint8_t rows;
extern const uint8_t cols;
extern const uint8_t pages;
extern const uint8_t barLength;

extern uint8_t seqCurrentOffset;
extern uint8_t seqCurrentLength;
extern uint8_t seqCurrentPage;

extern uint8_t resetMode;
uint8_t previousPage = 0;

extern int16_t potIn;
extern int8_t randomValuePotIn;
extern int8_t msDelay;
uint8_t markerRow = 0;
uint8_t markerCol = 0;

extern const uint8_t seqMatrixSize;
extern byte seqMatrix[seqMatrixSize];
extern uint8_t stepCount;
extern bool isPause;

extern Encoder Enc;
extern int8_t enc;
extern bool updateScreen;
extern bool updateTrigger;

extern Adafruit_SSD1306 display;

extern const char playText[];
extern const char pauseText[];
extern const char backText[];

extern const char lengthText[];
extern const char offsetText[];
extern const char pagesIndexText[];
extern const char slashText[];

extern const char *const resetOptions[];

enum SeqEncoderOptions { SEQ_ENC_PAGE = 96, SEQ_ENC_LENGTH, SEQ_ENC_OFFSET, SEQ_ENC_PLAY, SEQ_ENC_RESET, SEQ_ENC_BACK };

char seqBuffer[6];

extern uint8_t page;

extern bool clkMode;
extern bool outMode;
extern uint8_t bpm;
extern uint8_t intClock;

extern int8_t newPosition;
extern int8_t oldPosition;

extern bool buttonOn;
extern bool oldButton;
extern bool button;

extern bool resetIn;
extern bool oldResetIn;

void oledSeq() {
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
};

void toggleMatrixCell(int page, int row, int col) {
    if (BitUtils::isBitSet(page, row, col))
        BitUtils::clearBit(page, row, col);
    else
        BitUtils::setBit(page, row, col);
}

void sequencerLoop() {
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
                if (seqCurrentLength == pages) {
                    seqCurrentLength = 1;
                    return;
                }
                if (seqCurrentLength + seqCurrentOffset >= pages)
                    return;
                seqCurrentLength++;
                break;
            case SEQ_ENC_OFFSET:
                if (seqCurrentOffset == pages - 1)
                    seqCurrentOffset = 0;
                if (seqCurrentOffset + seqCurrentLength >= pages)
                    seqCurrentOffset = 0;
                seqCurrentOffset++;
                break;
            case SEQ_ENC_RESET:
                if (resetMode == 5)
                    resetMode = 0;
                else
                    resetMode++;
                break;
            case SEQ_ENC_PLAY:
                EncoderUtils::toggleParameter(isPause);
                break;
            case SEQ_ENC_BACK:
                page = 0;
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

    oledSeq();

    TriggerUtils::applyDelay(msDelay);
}

#endif