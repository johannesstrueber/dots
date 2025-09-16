#ifndef RANDOM_TRIGGER_H
#define RANDOM_TRIGGER_H

#include "utilities/Display.h"
#include "utilities/Encoder.h"
#include "utilities/MenuLayout.h"
#include "utilities/PatternMath.h"
#include "utilities/Trigger.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

extern bool updateScreen;
extern bool updateTrigger;
extern int8_t enc;
extern int8_t numChannels;

extern const uint8_t outputPins[];
extern bool resetIn;
extern bool oldResetIn;
extern bool buttonOn;
extern bool isPause;

extern int8_t newPosition;
extern int8_t oldPosition;

extern uint8_t page;

extern Adafruit_SSD1306 display;
extern Encoder Enc;

extern int8_t msDelay;
extern uint8_t bpm;
extern uint8_t intClock;
extern uint8_t stepCount;

extern bool outMode;
extern bool clkMode;
extern uint8_t ranMode;

extern const char playText[];
extern const char pauseText[];
extern const char backText[];
extern const char stepIndexText[];
extern const char channelIndexText[];

char ranBuffer[5];

bool channelTriggered[6] = {false, false, false, false, false, false};

enum RanEncoderOptions { RAN_ENC_CHANNELS = 7, RAN_ENC_MODE, RAN_ENC_PLAY, RAN_ENC_RESET, RAN_ENC_BACK };

extern const char *const resetOptions[];

enum RanModeOptions { RAN_MODE_ALL, RAN_MODE_SEQ, RAN_MODE_STEP };

extern const char *const ranModeOptions[];

extern uint8_t randomChannelValues[6];
extern uint8_t ranActiveChannels;

void saveRandomTriggerSettings() {
    EEPROM.write(395, ranMode);
    EEPROM.write(396, ranActiveChannels);
}

void loadRandomTriggerSettings() {
    ranMode = EEPROM.read(395);
    ranActiveChannels = EEPROM.read(396);

    // Validate ranges
    if (ranMode > 1)
        ranMode = 0;
    if (ranActiveChannels < 1 || ranActiveChannels > 6)
        ranActiveChannels = 6;
}

void oledRan() {
    DisplayUtils::initDisplay();

    for (int i = 0; i < ranActiveChannels; i++) {
        int x = 20 * i + 11;
        int y = 36 - randomChannelValues[i] * 3;

        if (channelTriggered[i]) {
            display.fillCircle(x, y, 7, WHITE);
            display.setTextColor(BLACK, WHITE);
        } else {
            display.drawCircle(x, y, 7, WHITE);
            display.setTextColor(WHITE, BLACK);
        }

        display.setCursor(x - 2, y - 3);
        display.print(i + 1);

        if (i == enc && !encLock)
            display.drawRoundRect(x - 7, 0, 15, 44, 8, WHITE);
        else {
            int lineEndY = y - 8;
            if (lineEndY > 0)
                display.drawFastVLine(x, 0, lineEndY, WHITE);
        }
    }

    if (ranMode == RAN_MODE_SEQ)
        display.fillRect(20 * stepCount * 1 + 9, 43, 5, 2, WHITE);

    DisplayUtils::drawMenuSeparator();

    DisplayUtils::drawNumberText(0, MenuLayout::MENU_Y_TOP, stepCount + 1, stepIndexText, false, ranBuffer);
    DisplayUtils::drawNumberText(24, MenuLayout::MENU_Y_TOP, ranActiveChannels, channelIndexText, enc == RAN_ENC_CHANNELS, ranBuffer);
    DisplayUtils::drawMenuItemFromArray(64, MenuLayout::MENU_Y_TOP, ranModeOptions, ranMode, enc == RAN_ENC_MODE, ranBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_TOP, isPause ? pauseText : playText, enc == RAN_ENC_PLAY, ranBuffer);

    DisplayUtils::drawDelay(0, MenuLayout::MENU_Y_BOTTOM, msDelay, ranBuffer);
    DisplayUtils::drawBPM(32, MenuLayout::MENU_Y_BOTTOM, bpm, intClock, clkMode, ranBuffer);
    DisplayUtils::drawMenuItemFromArray(64, MenuLayout::MENU_Y_BOTTOM, resetOptions, resetMode, enc == RAN_ENC_RESET, ranBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_BOTTOM, backText, enc == RAN_ENC_BACK, ranBuffer);
    display.display();
};

void randomLoop() {
    int8_t newPosition = Enc.read();
    static int8_t oldPosition = -2;
    static int8_t oldEnc = 0;
    updateScreen = false;

    if (msDelay < 0)
        TriggerUtils::applyDelay(msDelay);

    if (outMode)
        TriggerUtils::setAllOutputsLow();

    if (stepCount > ranActiveChannels - 1)
        stepCount = 0;

    EncoderUtils::handleEncoderBounds(enc, 0, RAN_ENC_BACK);

    if (enc >= ranActiveChannels && enc < RAN_ENC_CHANNELS) {
        if (enc > oldEnc) {
            enc = RAN_ENC_CHANNELS;
        } else {
            enc = ranActiveChannels - 1;
        }
    }

    oldEnc = enc;

    if (encLock) {
        if (newPosition < oldPosition && randomChannelValues[enc] > 0)
            randomChannelValues[enc]--;
        else if (newPosition > oldPosition && randomChannelValues[enc] < 10)
            randomChannelValues[enc]++;
        oldPosition = newPosition;
    }

    if (buttonOn)
        switch (enc) {
            case RAN_ENC_CHANNELS:
                if (ranActiveChannels == 6)
                    ranActiveChannels = 1;
                else
                    ranActiveChannels++;
                break;
            case RAN_ENC_MODE:
                if (ranMode == 1)
                    ranMode = 0;
                else
                    ranMode++;
                break;
            case RAN_ENC_PLAY:
                EncoderUtils::toggleParameter(isPause);
                break;
            case RAN_ENC_RESET:
                if (resetMode == 3)
                    resetMode = 0;
                else
                    resetMode++;
                break;
            case RAN_ENC_BACK:
                page = 0;
                updateScreen = true;
                break;
            default:
                EncoderUtils::toggleParameter(encLock);
                break;
        }

    oledRan();
    if (updateTrigger)
        for (int i = 0; i < ranActiveChannels; i++) {
            int trigger = random(0, 10) < randomChannelValues[i];
            switch (ranMode) {
                case RAN_MODE_ALL:
                    if (trigger) {
                        TriggerUtils::setOutput(i, true);
                        channelTriggered[i] = true;
                    } else {
                        TriggerUtils::setOutput(i, false);
                        channelTriggered[i] = false;
                    }
                    break;
                case RAN_MODE_SEQ:
                    if (trigger && i == stepCount) {
                        TriggerUtils::setOutput(i, true);
                        channelTriggered[i] = true;
                    } else {
                        TriggerUtils::setOutput(i, false);
                        channelTriggered[i] = false;
                    }
                    break;
            }
        }

    delay(30);
    TriggerUtils::applyDelay(msDelay);
}

#endif