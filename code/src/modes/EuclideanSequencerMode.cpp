#include "modes/EuclideanSequencerMode.h"

#include "State.h"
#include "UiStrings.h"
#include "utilities/DisplayUtils.h"
#include "utilities/EncoderUtils.h"
#include "utilities/MenuLayout.h"
#include "utilities/PatternMath.h"
#include "utilities/TriggerUtils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <EEPROM.h>

EuclideanTrack eucTracks[6] = {{16, 4, 0, false, 0}, {16, 3, 0, false, 0}, {12, 5, 0, false, 0}, {8, 3, 0, false, 0}, {16, 7, 0, false, 0}, {16, 2, 0, false, 0}};
uint8_t eucPatternLength = 16;

namespace {

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

uint8_t eucCurrentTrack = 0;
uint8_t eucCurrentStep = 0;

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

} // namespace

void updateEuclideanPatterns() {
    for (int i = 0; i < 6; i++) {
        uint32_t basePattern = PatternMath::generateEuclideanPattern(eucTracks[i].steps, eucTracks[i].hits);
        eucTracks[i].pattern = PatternMath::rotatePattern(basePattern, eucTracks[i].steps, eucTracks[i].rotation);
    }
}

void saveEuclideanSequencerSettings() {
    EEPROM.write(397, eucPatternLength);
    for (int i = 0; i < 6; i++) {
        int baseAddr = 398 + (i * 4);
        EEPROM.write(baseAddr, eucTracks[i].steps);
        EEPROM.write(baseAddr + 1, eucTracks[i].hits);
        EEPROM.write(baseAddr + 2, eucTracks[i].rotation);
        EEPROM.write(baseAddr + 3, eucTracks[i].mute);
    }
}

void loadEuclideanSequencerSettings() {
    eucPatternLength = EEPROM.read(397);
    for (int i = 0; i < 6; i++) {
        int baseAddr = 398 + (i * 4);
        eucTracks[i].steps = EEPROM.read(baseAddr);
        eucTracks[i].hits = EEPROM.read(baseAddr + 1);
        eucTracks[i].rotation = EEPROM.read(baseAddr + 2);
        eucTracks[i].mute = EEPROM.read(baseAddr + 3);
    }
}

void renderEuclideanSequencerScreen() {
    static char eucBuffer[5];

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

void updateEuclideanSequencer() {
    bool needsDisplayUpdate = false;
    int8_t oldEnc = enc;
    isPause = false;

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
                    if (eucTracks[eucCurrentTrack].steps > eucPatternLength)
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

                case EUC_ENC_LENGTH: {
                    eucPatternLength++;
                    if (eucPatternLength > 16)
                        eucPatternLength = 4;

                    bool anyTrackAdjusted = false;
                    for (int i = 0; i < 6; i++) {
                        if (eucTracks[i].steps > eucPatternLength) {
                            eucTracks[i].steps = eucPatternLength;
                            if (eucTracks[i].hits > eucTracks[i].steps) {
                                eucTracks[i].hits = eucTracks[i].steps;
                            }
                            anyTrackAdjusted = true;
                        }
                    }

                    if (anyTrackAdjusted) {
                        updateEuclideanPatterns();
                    }

                    needsDisplayUpdate = true;
                    break;
                }

                case EUC_ENC_RESET:
                    EncoderUtils::cycleEnum(resetMode, STEP_BACK);
                    needsDisplayUpdate = true;
                    break;

                case EUC_ENC_BACK:
                    page = MAIN_MENU;
                    updateScreen = true;
                    break;
            }
        }

        if (needsPatternUpdate) {
            uint32_t basePattern = PatternMath::generateEuclideanPattern(eucTracks[eucCurrentTrack].steps, eucTracks[eucCurrentTrack].hits);
            eucTracks[eucCurrentTrack].pattern = PatternMath::rotatePattern(basePattern, eucTracks[eucCurrentTrack].steps, eucTracks[eucCurrentTrack].rotation);
        }
    }

    TriggerUtils::handleStepOverflow(stepCount, eucPatternLength);
    if (stepCount == 0) {
        eucCurrentStep = 0;
    } else {
        eucCurrentStep = stepCount % eucPatternLength;
    }

    if (updateTrigger) {
        for (int i = 0; i < numChannels; i++) {
            uint8_t trackStep = eucCurrentStep % eucTracks[i].steps;
            bool shouldTrigger = !eucTracks[i].mute && isEuclideanStepActive(i, trackStep);

            TriggerUtils::setOutput(i, shouldTrigger);
        }
        updateTrigger = false;
        needsDisplayUpdate = true;
    }

    if (needsDisplayUpdate || updateScreen) {
        renderEuclideanSequencerScreen();
        updateScreen = false;
    }
}
