#ifndef EUCLIDEAN_SEQUENCER_H
#define EUCLIDEAN_SEQUENCER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <Wire.h>

#include "utilities/Display.h"
#include "utilities/Encoder.h"
#include "utilities/MenuLayout.h"
#include "utilities/PatternMath.h"
#include "utilities/Trigger.h"

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

struct EuclideanTrack {
    uint8_t steps;
    uint8_t hits;
    uint8_t rotation;
    bool mute;
    uint32_t pattern;
};

extern const char channelIndexText[];
extern const char stepIndexText[];
extern const char lengthIndexText[];
extern const char backText[];
extern const char playText[];
extern const char pauseText[];

extern const char eucHitsText[];
extern const char eucRotationText[];
extern const char eucMuteText[];
extern const char eucLiveText[];

char eucBuffer[5];

EuclideanTrack eucTracks[6] = {{16, 4, 0, false, 0}, {16, 3, 0, false, 0}, {12, 5, 0, false, 0}, {8, 3, 0, false, 0}, {16, 7, 0, false, 0}, {16, 2, 0, false, 0}};

uint8_t eucCurrentTrack = 0;
uint8_t eucPatternLength = 16;
uint8_t eucCurrentStep = 0;

enum EuclideanEncoderOptions {
    EUC_ENC_CHANNEL_1 = 1,
    EUC_ENC_CHANNEL_2,
    EUC_ENC_CHANNEL_3,
    EUC_ENC_CHANNEL_4,
    EUC_ENC_CHANNEL_5,
    EUC_ENC_CHANNEL_6,
    EUC_ENC_LENGTH,
    EUC_ENC_STEPS,
    EUC_ENC_HITS,
    EUC_ENC_ROTATION,
    EUC_ENC_MUTE,
    EUC_ENC_RESET,
    EUC_ENC_BACK
};

uint32_t generateEuclideanPattern(uint8_t steps, uint8_t hits) {
    return PatternMath::generateEuclideanPattern(steps, hits);
}

uint32_t rotatePattern(uint32_t pattern, uint8_t steps, uint8_t rotation) {
    return PatternMath::rotatePattern(pattern, steps, rotation);
}

void updateEuclideanPatterns() {
    for (int i = 0; i < 6; i++) {
        uint32_t basePattern = generateEuclideanPattern(eucTracks[i].steps, eucTracks[i].hits);
        eucTracks[i].pattern = rotatePattern(basePattern, eucTracks[i].steps, eucTracks[i].rotation);
    }
}

bool isEuclideanStepActive(uint8_t track, uint8_t step) {
    if (track >= 6)
        return false;
    if (step >= eucTracks[track].steps)
        return false;
    return PatternMath::isStepActive(eucTracks[track].pattern, step);
}

void drawPolygon(uint8_t centerX, uint8_t centerY, uint8_t radius, uint8_t steps, uint8_t currentStep, uint32_t pattern) {
    if (steps == 1) {

        display.fillCircle(centerX, centerY, 3, WHITE);
        return;
    } else if (steps == 2) {

        display.drawLine(centerX - 5, centerY, centerX + 5, centerY, WHITE);

        bool isActive1 = (pattern >> 0) & 1;
        bool isActive2 = (pattern >> 1) & 1;
        bool isCurrent1 = (0 == currentStep);
        bool isCurrent2 = (1 == currentStep);

        if (isCurrent1) {
            display.fillCircle(centerX - 5, centerY, 2, WHITE);
            if (!isActive1)
                display.fillCircle(centerX - 5, centerY, 1, BLACK);
        } else if (isActive1) {
            display.fillCircle(centerX - 5, centerY, 1, WHITE);
        }

        if (isCurrent2) {
            display.fillCircle(centerX + 5, centerY, 2, WHITE);
            if (!isActive2)
                display.fillCircle(centerX + 5, centerY, 1, BLACK);
        } else if (isActive2) {
            display.fillCircle(centerX + 5, centerY, 1, WHITE);
        }
        return;
    }

    if (steps < 3)
        steps = 3;
    if (steps > 16)
        steps = 16;

    for (int i = 0; i < steps; i++) {
        float angle1 = (2.0 * PI * i) / steps - PI / 2;
        float angle2 = (2.0 * PI * (i + 1)) / steps - PI / 2;

        int x1 = centerX + radius * cos(angle1);
        int y1 = centerY + radius * sin(angle1);
        int x2 = centerX + radius * cos(angle2);
        int y2 = centerY + radius * sin(angle2);

        display.drawLine(x1, y1, x2, y2, WHITE);
    }

    for (int i = 0; i < steps; i++) {
        float angle = (2.0 * PI * i) / steps - PI / 2;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        bool isActive = (pattern >> i) & 1;
        bool isCurrent = (i == currentStep);

        if (isCurrent) {
            display.fillCircle(x, y, 3, WHITE);
            if (isActive) {
                display.fillCircle(x, y, 2, BLACK);
                display.fillCircle(x, y, 1, WHITE);
            } else {
                display.fillCircle(x, y, 1, BLACK);
            }
        } else if (isActive) {
            display.fillCircle(x, y, 2, WHITE);
        } else {
            display.drawCircle(x, y, 1, WHITE);
        }
    }
}

void oledEuclidean() {
    DisplayUtils::initDisplay();

    for (int ch = 0; ch < 6; ch++) {
        uint8_t chX, chY;
        MenuLayout::getChannelPosition(ch, chX, chY);

        bool isEncoderHover = (enc == ch + 1);
        bool isActiveTrack = (ch == eucCurrentTrack);
        bool hasCurrentStep = (eucCurrentStep % eucTracks[ch].steps) < eucTracks[ch].steps;
        bool isTriggering = hasCurrentStep && isEuclideanStepActive(ch, eucCurrentStep % eucTracks[ch].steps) && !eucTracks[ch].mute;

        if (eucTracks[ch].mute) {
            if (isActiveTrack || isEncoderHover) {
                display.drawCircle(chX, chY, 9, WHITE);
            }
        } else {
            if (isTriggering) {
                display.fillCircle(chX, chY, 7, WHITE);
                display.setTextColor(BLACK, WHITE);
            } else {
                display.drawCircle(chX, chY, 7, WHITE);
                display.setTextColor(WHITE, BLACK);
            }

            if (isActiveTrack || isEncoderHover) {
                display.drawCircle(chX, chY, 9, WHITE);
            }

            display.setCursor(chX - 2, chY - 3);
            display.print(ch + 1);
        }
    }

    display.drawFastVLine(MenuLayout::VERTICAL_SEPARATOR_X, 0, 45, WHITE);

    uint8_t centerX, centerY;
    MenuLayout::getGraphicsCenter(centerX, centerY);
    uint8_t radius = min(MenuLayout::GRAPHICS_AREA_WIDTH, MenuLayout::GRAPHICS_AREA_HEIGHT) / 3;

    drawPolygon(centerX, centerY, radius, eucTracks[eucCurrentTrack].steps, eucCurrentStep % eucTracks[eucCurrentTrack].steps, eucTracks[eucCurrentTrack].pattern);

    DisplayUtils::drawMenuSeparator();

    DisplayUtils::drawNumberText(0, MenuLayout::MENU_Y_TOP, eucPatternLength, lengthIndexText, enc == EUC_ENC_LENGTH, eucBuffer);
    DisplayUtils::drawNumberText(24, MenuLayout::MENU_Y_TOP, eucTracks[eucCurrentTrack].steps, stepIndexText, enc == EUC_ENC_STEPS, eucBuffer);
    DisplayUtils::drawNumberText(48, MenuLayout::MENU_Y_TOP, eucTracks[eucCurrentTrack].hits, eucHitsText, enc == EUC_ENC_HITS, eucBuffer);
    DisplayUtils::drawNumberText(72, MenuLayout::MENU_Y_TOP, eucTracks[eucCurrentTrack].rotation, eucRotationText, enc == EUC_ENC_ROTATION, eucBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_TOP, eucTracks[eucCurrentTrack].mute ? eucMuteText : eucLiveText, enc == EUC_ENC_MUTE, eucBuffer);

    DisplayUtils::drawDelay(0, MenuLayout::MENU_Y_BOTTOM, msDelay, eucBuffer);
    DisplayUtils::drawBPM(32, MenuLayout::MENU_Y_BOTTOM, bpm, intClock, clkMode, eucBuffer);
    DisplayUtils::drawMenuItemFromArray(64, MenuLayout::MENU_Y_BOTTOM, resetOptions, resetMode, enc == EUC_ENC_RESET, eucBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_BOTTOM, backText, enc == EUC_ENC_BACK, eucBuffer);

    display.display();
}

void saveEuclideanSettings() {
    EEPROM.write(397, eucPatternLength);
    for (int i = 0; i < 6; i++) {
        int baseAddr = 398 + (i * 4);
        EEPROM.write(baseAddr, eucTracks[i].steps);
        EEPROM.write(baseAddr + 1, eucTracks[i].hits);
        EEPROM.write(baseAddr + 2, eucTracks[i].rotation);
        EEPROM.write(baseAddr + 3, eucTracks[i].mute);
    }
}

void loadEuclideanSettings() {
    eucPatternLength = EEPROM.read(397);
    for (int i = 0; i < 6; i++) {
        int baseAddr = 398 + (i * 4);
        eucTracks[i].steps = EEPROM.read(baseAddr);
        eucTracks[i].hits = EEPROM.read(baseAddr + 1);
        eucTracks[i].rotation = EEPROM.read(baseAddr + 2);
        eucTracks[i].mute = EEPROM.read(baseAddr + 3);
    }
}

void euclideanLoop() {
    bool needsDisplayUpdate = false;
    int8_t oldEnc = enc;

    EncoderUtils::handleEncoderBounds(enc, EUC_ENC_CHANNEL_1, EUC_ENC_BACK);

    if (enc != oldEnc) {
        needsDisplayUpdate = true;
    }

    if (buttonOn) {
        bool needsPatternUpdate = false;

        if (enc >= EUC_ENC_CHANNEL_1 && enc <= EUC_ENC_CHANNEL_6) {
            eucCurrentTrack = enc - 1;
            enc = EUC_ENC_LENGTH;
            needsDisplayUpdate = true;
        } else {
            switch (enc) {
                case EUC_ENC_STEPS:
                    eucTracks[eucCurrentTrack].steps++;
                    if (eucTracks[eucCurrentTrack].steps > 12)
                        eucTracks[eucCurrentTrack].steps = 1;
                    if (eucTracks[eucCurrentTrack].hits > eucTracks[eucCurrentTrack].steps) {
                        eucTracks[eucCurrentTrack].hits = eucTracks[eucCurrentTrack].steps;
                    }
                    needsPatternUpdate = true;
                    needsDisplayUpdate = true;
                    break;

                case EUC_ENC_HITS:
                    eucTracks[eucCurrentTrack].hits++;
                    if (eucTracks[eucCurrentTrack].hits > eucTracks[eucCurrentTrack].steps) {
                        eucTracks[eucCurrentTrack].hits = 0;
                    }
                    needsPatternUpdate = true;
                    needsDisplayUpdate = true;
                    break;

                case EUC_ENC_ROTATION:
                    eucTracks[eucCurrentTrack].rotation++;
                    if (eucTracks[eucCurrentTrack].rotation >= eucTracks[eucCurrentTrack].steps) {
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

                case EUC_ENC_RESET:
                    EncoderUtils::cycleEnum(resetMode, 2);
                    needsDisplayUpdate = true;
                    break;

                case EUC_ENC_BACK:
                    page = 0;
                    updateScreen = true;
                    return;
            }
        }

        if (needsPatternUpdate) {
            eucTracks[eucCurrentTrack].pattern = PatternMath::generateEuclideanPattern(eucTracks[eucCurrentTrack].steps, eucTracks[eucCurrentTrack].hits);
        }
    }

    TriggerUtils::handleStepOverflow(stepCount, eucPatternLength);
    if (stepCount == 0) {
        eucCurrentStep = 0;
    } else {
        eucCurrentStep = stepCount;
    }

    if (!isPause && updateTrigger) {
        for (int i = 0; i < numChannels; i++) {
            bool shouldTrigger = !eucTracks[i].mute && isEuclideanStepActive(i, eucCurrentStep);

            TriggerUtils::setOutput(i, shouldTrigger);
        }
        updateTrigger = false;
        needsDisplayUpdate = true;
    }

    if (needsDisplayUpdate || updateScreen) {
        oledEuclidean();
        updateScreen = false;
    }
}

#endif
