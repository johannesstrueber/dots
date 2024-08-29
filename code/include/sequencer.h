#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

extern const char *const resetOptions[];

enum SeqEncoderOptions
{
    SEQ_ENC_PAGE = 96,
    SEQ_ENC_LENGTH,
    SEQ_ENC_OFFSET,
    SEQ_ENC_PLAY,
    SEQ_ENC_RESET,
    SEQ_ENC_BACK
};

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

#include "utils/SetBit.h"
#include "utils/ClearBit.h"
#include "utils/IsBitSet.h"

void oledSeq()
{
    display.setCursor(0, 0);

    if (enc > 0 || enc < SEQ_ENC_PAGE || previousPage != seqCurrentPage)
    {
        display.clearDisplay();
        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                bool isActive = isBitSet(seqCurrentPage - 1, row, col);
                uint8_t x = col * 8;
                uint8_t y = row * 7;

                if (isActive)
                {
                    display.drawFastVLine(x + RECT_WIDTH, y + 2, RECT_HEIGHT - 4, WHITE);
                    display.drawFastHLine(x + 3, y + RECT_HEIGHT, RECT_WIDTH - 5, WHITE);
                }
                else
                    display.fillRoundRect(x, y, RECT_WIDTH + 1, RECT_HEIGHT, 5, WHITE);
            }
        }
        if (enc < SEQ_ENC_PAGE)
        {
            // Draw the position marker
            uint8_t markerX = markerCol * 8;
            uint8_t markerY = markerRow * 7;

            // draw black background
            bool color = isBitSet(seqCurrentPage - 1, markerRow, markerCol);

            display.fillRoundRect(markerX + 1, markerY + 1, RECT_WIDTH - 1, RECT_HEIGHT - 1, 5, color ? BLACK : WHITE);
            display.fillRoundRect(markerX + 2, markerY + 2, RECT_WIDTH - 3, RECT_HEIGHT - 3, 3, color ? WHITE : BLACK);
        }
    }

    display.drawFastVLine(0, 0, 41, BLACK);
    display.fillRect(0, 42, 128, 3, BLACK);
    for (int i = 1; i * barLength < cols; i++)
    {
        uint8_t xPosition = i * barLength * 8;
        display.fillRect(xPosition, 43, 1, 2, WHITE);
    }
    display.fillRect((stepCount + 1) * 8 - 6, 43, 4, 2, WHITE);

    display.setCursor(0, 48);
    display.setTextColor((enc == SEQ_ENC_PAGE) ? BLACK : WHITE, (enc == SEQ_ENC_PAGE) ? WHITE : BLACK);
    display.print(seqCurrentPage);
    display.print("/");
    display.print(pages);
    display.print("P");

    display.setCursor(32, 48);
    display.setTextColor((enc == SEQ_ENC_LENGTH) ? BLACK : WHITE, (enc == SEQ_ENC_LENGTH) ? WHITE : BLACK);
    display.print(seqCurrentLength);
    strcpy_P(seqBuffer, lengthText);
    display.print(seqBuffer);

    display.setCursor(64, 48);
    display.setTextColor((enc == SEQ_ENC_OFFSET) ? BLACK : WHITE, (enc == SEQ_ENC_OFFSET) ? WHITE : BLACK);
    display.print(seqCurrentOffset);
    strcpy_P(seqBuffer, offsetText);
    display.print(seqBuffer);

    display.setCursor(96, 48);
    display.setTextColor((enc == SEQ_ENC_PLAY) ? BLACK : WHITE, (enc == SEQ_ENC_PLAY) ? WHITE : BLACK);
    strcpy_P(seqBuffer, isPause ? pauseText : playText);
    display.print(seqBuffer);

    display.setCursor(0, 57);
    display.setTextColor(WHITE, BLACK);
    display.print(msDelay < 0 ? msDelay * -1 : msDelay);
    display.setCursor(18, 57);
    display.print("D");

    display.setCursor(32, 57);
    display.print(clkMode ? intClock : bpm);
    display.print("B");

    display.setCursor(64, 57);
    display.setTextColor((enc == SEQ_ENC_RESET) ? BLACK : WHITE, (enc == SEQ_ENC_RESET) ? WHITE : BLACK);
    strcpy_P(seqBuffer, (char *)pgm_read_word(&(resetOptions[resetMode])));
    display.print(seqBuffer);

    display.setCursor(96, 57);
    display.setTextColor((enc == SEQ_ENC_BACK) ? BLACK : WHITE, (enc == SEQ_ENC_BACK) ? WHITE : BLACK);
    strcpy_P(seqBuffer, backText);
    display.print(seqBuffer);

    display.display();
};

void toggleMatrixCell(int page, int row, int col)
{
    if (isBitSet(page, row, col))
        clearBit(page, row, col);
    else
        setBit(page, row, col);
}

void sequencerLoop()
{
    updateScreen = false;

    if (msDelay < 0)
        delay(msDelay * -1);

    if (enc > SEQ_ENC_BACK)
        enc = 0;
    else if (enc < 0)
        enc = SEQ_ENC_BACK;

    if (enc < cols * rows)
    {
        markerCol = enc % cols;
        markerRow = enc / cols;
    }

    if (buttonOn)
        switch (enc)
        {
        case SEQ_ENC_PAGE:
            previousPage = seqCurrentPage;
            seqCurrentPage = seqCurrentPage >= pages ? 1 : seqCurrentPage + 1;
            break;
        case SEQ_ENC_LENGTH:
            if (seqCurrentLength == pages)
            {
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
            isPause = !isPause;
            break;
        case SEQ_ENC_BACK:
            page = 0;
            updateScreen = true;
            break;
        default:
            toggleMatrixCell(seqCurrentPage - 1, markerRow, markerCol);
            break;
        }

    if (stepCount >= cols)
    {
        if (seqCurrentPage <= pages)
        {
            if (seqCurrentOffset + seqCurrentLength <= seqCurrentPage)
                seqCurrentPage = seqCurrentOffset + 1;
            else
                seqCurrentPage++;
        }
        else
            seqCurrentPage = seqCurrentOffset + 1;
        previousPage = seqCurrentPage;
        stepCount = 0;
    }

    if (!isPause && updateTrigger)
        for (int i = 0; i < numChannels; i++)
        {
            if (!isBitSet(seqCurrentPage - 1, i, stepCount))
                digitalWrite(outputPins[i], HIGH);
            else
                digitalWrite(outputPins[i], LOW);
        }

    oledSeq();

    if (msDelay > 0)
        delay(msDelay);
}

#endif