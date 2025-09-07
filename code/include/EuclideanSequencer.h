#ifndef EUCLIDEAN_SEQUENCER_H
#define EUCLIDEAN_SEQUENCER_H

#include <Arduino.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Shared utilities
#include "utilities/Display.h"
#include "utilities/Encoder.h"
#include "utilities/MenuLayout.h"
#include "utilities/Trigger.h"
#include "utilities/PatternMath.h"

extern Adafruit_SSD1306 display;
extern Encoder Enc;
extern int8_t enc;
extern bool updateScreen;
extern bool updateTrigger;
extern bool buttonOn;
extern uint8_t stepCount;
extern uint8_t page;
extern bool isPause;
extern const uint8_t outputPins[];
extern int8_t numChannels;
extern int16_t potIn;
extern int8_t msDelay;

struct EuclideanTrack
{
    uint8_t steps;
    uint8_t hits;
    uint8_t rotation;
    bool mute;
    uint32_t pattern;
};

EuclideanTrack eucTracks[6] = {
    {16, 4, 0, false, 0},
    {16, 3, 0, false, 0},
    {12, 5, 0, false, 0},
    {8, 3, 0, false, 0},
    {16, 7, 0, false, 0},
    {16, 2, 0, false, 0}};

uint8_t eucCurrentTrack = 0;
uint8_t eucPatternLength = 16;
uint8_t eucCurrentStep = 0;

enum EuclideanEncoderOptions
{
    EUC_ENC_TRACK = 96,
    EUC_ENC_STEPS,
    EUC_ENC_HITS,
    EUC_ENC_ROTATION,
    EUC_ENC_MUTE,
    EUC_ENC_LENGTH,
    EUC_ENC_PLAY,
    EUC_ENC_BACK
};

uint32_t generateEuclideanPattern(uint8_t steps, uint8_t hits)
{
    return PatternMath::generateEuclideanPattern(steps, hits);
}

uint32_t rotatePattern(uint32_t pattern, uint8_t steps, uint8_t rotation)
{
    return PatternMath::rotatePattern(pattern, steps, rotation);
}

void updateEuclideanPatterns()
{
    for (int i = 0; i < 6; i++)
    {
        uint32_t basePattern = generateEuclideanPattern(eucTracks[i].steps, eucTracks[i].hits);
        eucTracks[i].pattern = rotatePattern(basePattern, eucTracks[i].steps, eucTracks[i].rotation);
    }
}

bool isEuclideanStepActive(uint8_t track, uint8_t step)
{
    if (track >= 6)
        return false;
    if (step >= eucTracks[track].steps)
        return false;
    return PatternMath::isStepActive(eucTracks[track].pattern, step);
}

void drawPolygon(uint8_t centerX, uint8_t centerY, uint8_t radius, uint8_t steps, uint8_t currentStep, uint32_t pattern)
{
    if (steps == 1)
    {

        display.fillCircle(centerX, centerY, 3, WHITE);
        return;
    }
    else if (steps == 2)
    {

        display.drawLine(centerX - 5, centerY, centerX + 5, centerY, WHITE);

        bool isActive1 = (pattern >> 0) & 1;
        bool isActive2 = (pattern >> 1) & 1;
        bool isCurrent1 = (0 == currentStep);
        bool isCurrent2 = (1 == currentStep);

        if (isCurrent1)
        {
            display.fillCircle(centerX - 5, centerY, 2, WHITE);
            if (!isActive1)
                display.fillCircle(centerX - 5, centerY, 1, BLACK);
        }
        else if (isActive1)
        {
            display.fillCircle(centerX - 5, centerY, 1, WHITE);
        }

        if (isCurrent2)
        {
            display.fillCircle(centerX + 5, centerY, 2, WHITE);
            if (!isActive2)
                display.fillCircle(centerX + 5, centerY, 1, BLACK);
        }
        else if (isActive2)
        {
            display.fillCircle(centerX + 5, centerY, 1, WHITE);
        }
        return;
    }

    if (steps < 3)
        steps = 3;
    if (steps > 16)
        steps = 16;

    for (int i = 0; i < steps; i++)
    {
        float angle1 = (2.0 * PI * i) / steps - PI / 2;
        float angle2 = (2.0 * PI * (i + 1)) / steps - PI / 2;

        int x1 = centerX + radius * cos(angle1);
        int y1 = centerY + radius * sin(angle1);
        int x2 = centerX + radius * cos(angle2);
        int y2 = centerY + radius * sin(angle2);

        display.drawLine(x1, y1, x2, y2, WHITE);
    }

    for (int i = 0; i < steps; i++)
    {
        float angle = (2.0 * PI * i) / steps - PI / 2;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        bool isActive = (pattern >> i) & 1;
        bool isCurrent = (i == currentStep);

        if (isCurrent)
        {

            display.fillCircle(x, y, 3, WHITE);
            if (isActive)
            {
                display.fillCircle(x, y, 2, BLACK);
                display.fillCircle(x, y, 1, WHITE);
            }
            else
            {
                display.fillCircle(x, y, 1, BLACK);
            }
        }
        else if (isActive)
        {

            display.fillCircle(x, y, 2, WHITE);
        }
        else
        {

            display.drawCircle(x, y, 1, WHITE);
        }
    }
}

void oledEuclidean()
{
    DisplayUtils::initDisplay();

    for (int ch = 0; ch < 6; ch++)
    {
        if (eucTracks[ch].mute)
            continue;

        uint8_t chX, chY;
        MenuLayout::getChannelPosition(ch, chX, chY);

        bool isCurrentTrack = (ch == eucCurrentTrack);
        bool isSelected = (enc == EUC_ENC_TRACK && isCurrentTrack);
        bool hasCurrentStep = (eucCurrentStep % eucTracks[ch].steps) < eucTracks[ch].steps;
        bool isTriggering = hasCurrentStep &&
                            isEuclideanStepActive(ch, eucCurrentStep % eucTracks[ch].steps);

        if (isTriggering)
        {
            display.fillCircle(chX, chY, 7, WHITE);
            display.setTextColor(BLACK, WHITE);
        }
        else
        {
            display.drawCircle(chX, chY, 7, WHITE);
            display.setTextColor(WHITE, BLACK);
        }

        if (isSelected)
        {
            display.drawCircle(chX, chY, 9, WHITE);
        }

        display.setCursor(chX - 2, chY - 3);
        display.print(ch + 1);
    }

    display.drawFastVLine(MenuLayout::VERTICAL_SEPARATOR_X, 0, 45, WHITE);

    uint8_t centerX, centerY;
    MenuLayout::getGraphicsCenter(centerX, centerY);
    uint8_t radius = min(MenuLayout::GRAPHICS_AREA_WIDTH, MenuLayout::GRAPHICS_AREA_HEIGHT) / 3;

    drawPolygon(centerX, centerY, radius,
                eucTracks[eucCurrentTrack].steps,
                eucCurrentStep % eucTracks[eucCurrentTrack].steps,
                eucTracks[eucCurrentTrack].pattern);

    DisplayUtils::drawMenuSeparator();

    // Top row menu items
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL1, MenuLayout::MENU_Y_TOP,
                               "CH", enc == EUC_ENC_TRACK);
    display.print(eucCurrentTrack + 1);
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL2, MenuLayout::MENU_Y_TOP,
                               "S", enc == EUC_ENC_STEPS);
    display.print(eucTracks[eucCurrentTrack].steps);
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL3, MenuLayout::MENU_Y_TOP,
                               "H", enc == EUC_ENC_HITS);
    display.print(eucTracks[eucCurrentTrack].hits);
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL4, MenuLayout::MENU_Y_TOP,
                               "R", enc == EUC_ENC_ROTATION);
    display.print(eucTracks[eucCurrentTrack].rotation);
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL5, MenuLayout::MENU_Y_TOP,
                               eucTracks[eucCurrentTrack].mute ? "MUTE" : "LIVE",
                               enc == EUC_ENC_MUTE);

    // Bottom row menu items
    display.setCursor(MenuLayout::MENU_X_COL1, MenuLayout::MENU_Y_BOTTOM);
    display.setTextColor(enc == EUC_ENC_LENGTH ? BLACK : WHITE,
                         enc == EUC_ENC_LENGTH ? WHITE : BLACK);
    display.print("L");
    display.print(eucPatternLength);
    DisplayUtils::drawDelayDisplay(MenuLayout::MENU_X_COL2, MenuLayout::MENU_Y_BOTTOM, msDelay);
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL4, MenuLayout::MENU_Y_BOTTOM,
                               isPause ? "PLAY" : "PAUSE", enc == EUC_ENC_PLAY);
    DisplayUtils::drawMenuItem(MenuLayout::MENU_X_COL5, MenuLayout::MENU_Y_BOTTOM,
                               "BACK", enc == EUC_ENC_BACK);

    display.display();
}

void euclideanLoop()
{
    bool needsDisplayUpdate = false;
    int8_t oldEnc = enc;

    EncoderUtils::handleEncoderBounds(enc, EUC_ENC_TRACK, EUC_ENC_BACK);

    if (enc != oldEnc)
    {
        needsDisplayUpdate = true;
    }

    if (buttonOn)
    {
        bool needsPatternUpdate = false;

        switch (enc)
        {
        case EUC_ENC_TRACK:
            eucCurrentTrack = (eucCurrentTrack + 1) % 6;
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_STEPS:
            eucTracks[eucCurrentTrack].steps++;
            if (eucTracks[eucCurrentTrack].steps > 12)
                eucTracks[eucCurrentTrack].steps = 1;
            if (eucTracks[eucCurrentTrack].hits > eucTracks[eucCurrentTrack].steps)
            {
                eucTracks[eucCurrentTrack].hits = eucTracks[eucCurrentTrack].steps;
            }
            needsPatternUpdate = true;
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_HITS:
            eucTracks[eucCurrentTrack].hits++;
            if (eucTracks[eucCurrentTrack].hits > eucTracks[eucCurrentTrack].steps)
            {
                eucTracks[eucCurrentTrack].hits = 0;
            }
            needsPatternUpdate = true;
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_ROTATION:
            eucTracks[eucCurrentTrack].rotation++;
            if (eucTracks[eucCurrentTrack].rotation >= eucTracks[eucCurrentTrack].steps)
            {
                eucTracks[eucCurrentTrack].rotation = 0;
            }
            needsPatternUpdate = true;
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_MUTE:
            EncoderUtils::toggleParameter(eucTracks[eucCurrentTrack].mute);
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_LENGTH:
            eucPatternLength++;
            if (eucPatternLength > 12)
                eucPatternLength = 4;
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_PLAY:
            EncoderUtils::toggleParameter(isPause);
            needsDisplayUpdate = true;
            break;

        case EUC_ENC_BACK:
            page = 0;
            updateScreen = true;
            return;
        }

        if (needsPatternUpdate)
        {
            eucTracks[eucCurrentTrack].pattern = PatternMath::generateEuclideanPattern(
                eucTracks[eucCurrentTrack].steps,
                eucTracks[eucCurrentTrack].hits);
        }
    }

    TriggerUtils::handleStepOverflow(stepCount, eucPatternLength);
    if (stepCount == 0)
    {
        eucCurrentStep = 0;
    }
    else
    {
        eucCurrentStep = stepCount;
    }

    if (!isPause && updateTrigger)
    {
        for (int i = 0; i < numChannels; i++)
        {
            bool shouldTrigger = !eucTracks[i].mute &&
                                 isEuclideanStepActive(i, eucCurrentStep);

            TriggerUtils::setOutput(i, shouldTrigger);
        }
        updateTrigger = false;
        needsDisplayUpdate = true;
    }

    if (needsDisplayUpdate || updateScreen)
    {
        oledEuclidean();
        updateScreen = false;
    }
}

#endif
