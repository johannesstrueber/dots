#include "modes/ClockDividerMode.h"

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

namespace {

enum DivEncoderOptions { DIV_ENC_CH1, DIV_ENC_CH2, DIV_ENC_CH3, DIV_ENC_CH4, DIV_ENC_CH5, DIV_ENC_CH6, DIV_ENC_PLAY, DIV_ENC_RESET, DIV_ENC_BACK };

uint8_t channelDividers[6] = {1, 2, 4, 3, 6, 8};

} // namespace

void saveClockDividerSettings() {
    for (int i = 0; i < 6; i++) {
        EEPROM.write(422 + i, channelDividers[i]);
    }
}

void loadClockDividerSettings() {
    for (int i = 0; i < 6; i++) {
        uint8_t value = EEPROM.read(422 + i);
        if (value >= 1 && value <= 16) {
            channelDividers[i] = value;
        } else {
            channelDividers[i] = (i == 0) ? 1 : (1 << (i % 4 + 1));
        }
    }
}

void renderClockDividerScreen() {
    static char divBuffer[5];

    DisplayUtils::initDisplay();

    DisplayUtils::drawMenuSeparator();

    DisplayUtils::drawNumberText(0, MenuLayout::MENU_Y_TOP, enc >= DIV_ENC_CH1 && enc <= DIV_ENC_CH6 ? (enc - DIV_ENC_CH1 + 1) : 0, channelIndexText, enc >= DIV_ENC_CH1 && enc <= DIV_ENC_CH6,
                                 divBuffer);
    DisplayUtils::drawStepCounter(32, MenuLayout::MENU_Y_TOP, stepCount, divBuffer);

    DisplayUtils::drawMenuItemProgMem(64, MenuLayout::MENU_Y_TOP, unselectedText, false, divBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_TOP, isPause ? pauseText : playText, enc == DIV_ENC_PLAY, divBuffer);

    DisplayUtils::drawDelay(0, MenuLayout::MENU_Y_BOTTOM, msDelay, divBuffer);
    DisplayUtils::drawBPM(32, MenuLayout::MENU_Y_BOTTOM, bpm, intClock, clkMode, divBuffer);
    DisplayUtils::drawMenuItemFromArray(64, MenuLayout::MENU_Y_BOTTOM, resetOptions, resetMode, enc == DIV_ENC_RESET, divBuffer);
    DisplayUtils::drawMenuItemProgMem(96, MenuLayout::MENU_Y_BOTTOM, backText, enc == DIV_ENC_BACK, divBuffer);

    for (int i = 0; i < numChannels; i++) {
        int x = (i >= 3) ? (i - 3) * 22 + 40 : i * 22 + 40;
        int y = (i >= 3) ? 32 : 10;

        bool shouldFill = (stepCount % channelDividers[i] == 0);
        bool isChannelSelected = (enc == DIV_ENC_CH1 + i);

        if (shouldFill) {
            display.fillCircle(x, y, 9, WHITE);
            display.setTextColor(BLACK, WHITE);
        } else {
            display.drawCircle(x, y, 9, WHITE);
            display.setTextColor(WHITE, BLACK);
        }

        if (isChannelSelected) {
            display.drawCircle(x, y, 11, WHITE);
        }

        if (channelDividers[i] > 9) {
            display.setCursor(x - 6, y - 3);
        } else {
            display.setCursor(x - 2, y - 3);
        }
        display.print(channelDividers[i]);
    }
    display.display();
}

void updateClockDivider() {
    bool needsDisplayUpdate = false;
    int8_t oldEnc = enc;

    if (msDelay < 0)
        TriggerUtils::applyDelay(msDelay);

    oldResetIn = resetIn;
    oldButton = button;

    if (outMode)
        TriggerUtils::setAllOutputsLow();

    EncoderUtils::handleEncoderBounds(enc, 0, DIV_ENC_BACK);

    if (enc != oldEnc) {
        needsDisplayUpdate = true;
    }

    if (buttonOn) {
        switch (enc) {
            case DIV_ENC_PLAY:
                EncoderUtils::toggleParameter(isPause);
                needsDisplayUpdate = true;
                break;
            case DIV_ENC_CH1:
            case DIV_ENC_CH2:
            case DIV_ENC_CH3:
            case DIV_ENC_CH4:
            case DIV_ENC_CH5:
            case DIV_ENC_CH6: {
                int channelIndex = enc - DIV_ENC_CH1;
                channelDividers[channelIndex]++;
                if (channelDividers[channelIndex] > 16) {
                    channelDividers[channelIndex] = 1;
                }
                saveClockDividerSettings();
                needsDisplayUpdate = true;
            } break;
            case DIV_ENC_RESET:
                if (resetMode == NONE)
                    resetMode = RESET;
                else
                    resetMode++;
                needsDisplayUpdate = true;
                break;
            case DIV_ENC_BACK:
                page = MAIN_MENU;
                updateScreen = true;
                return;
        }
    }

    if (resetIn && !oldResetIn) {
        stepCount = 0;
        oldResetIn = resetIn;
    }

    if (stepCount == 64)
        stepCount = 0;

    if (updateTrigger) {
        for (int i = 0; i < numChannels; i++) {
            bool shouldTrigger = (stepCount % channelDividers[i] == 0);
            TriggerUtils::setOutput(i, shouldTrigger);
        }
        updateTrigger = false;
        needsDisplayUpdate = true;
    }

    if (needsDisplayUpdate || updateScreen) {
        renderClockDividerScreen();
        updateScreen = false;
    }

    TriggerUtils::applyDelay(msDelay);
}
