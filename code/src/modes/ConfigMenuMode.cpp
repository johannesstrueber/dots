#include "modes/ConfigMenuMode.h"

#include "State.h"
#include "UiStrings.h"
#include "modes/ClockDividerMode.h"
#include "modes/EuclideanSequencerMode.h"
#include "modes/RandomTriggerMode.h"
#include "modes/StepSequencerMode.h"
#include "utilities/DisplayUtils.h"
#include "utilities/EncoderUtils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>

namespace {
enum ConfigOptions { CLK, BPM, OUT, BOOT, SAVE, BACK };
constexpr int8_t CONFIG_MENU_OPTION_COUNT = 6;
} // namespace

void renderConfigMenuScreen() {
    static char configBuffer[5];

    DisplayUtils::initDisplay();

    for (int i = 0; i < CONFIG_MENU_OPTION_COUNT; i++) {
        DisplayUtils::drawMenuItemFromArray(0, i * 9, configMenuOptions, i, enc == i, configBuffer);
    }

    display.setTextColor(WHITE, BLACK);
    display.setCursor(64, 0);
    strcpy_P(configBuffer, clkMode ? clkOptionInt : clkOptionExt);
    display.print(configBuffer);

    display.setCursor(64, 9);
    display.print(clkMode ? intClock : bpm);

    display.setCursor(64, 18);
    strcpy_P(configBuffer, outMode ? outOptionTrig : outOptionGate);
    display.print(configBuffer);

    display.setCursor(64, 27);
    strcpy_P(configBuffer, (char *)pgm_read_word(&(bootOptions[bootMode])));
    display.print(configBuffer);

    display.display();
}

void updateConfigMenu() {
    int8_t newPosition = Enc.read();
    static int8_t oldPosition = -2;

    if (encLock) {
        if (newPosition < oldPosition)
            intClock--;
        else if (newPosition > oldPosition)
            intClock++;
        updateScreen = true;
        oldPosition = newPosition;
    }

    if (intClock > 200)
        intClock = 200;
    else if (intClock < 10)
        intClock = 10;

    EncoderUtils::handleEncoderBounds(enc, 0, CONFIG_MENU_OPTION_COUNT - 1);

    if (buttonOn)
        switch (enc) {
            case CLK:
                EncoderUtils::toggleParameter(clkMode);
                break;
            case BPM:
                EncoderUtils::toggleParameter(encLock);
                enc = 1;
                break;
            case OUT:
                EncoderUtils::toggleParameter(outMode);
                break;
            case BOOT:
                EncoderUtils::cycleEnum(bootMode, 4);
                break;
            case SAVE:
                // config settings
                EEPROM.write(387, resetMode);
                EEPROM.write(388, isPause);
                EEPROM.write(390, clkMode);
                EEPROM.write(391, intClock);
                EEPROM.write(392, bootMode);
                EEPROM.write(393, outMode);

                // program settings
                saveStepSequencerSettings();
                saveRandomTriggerSettings();
                saveEuclideanSequencerSettings();
                saveClockDividerSettings();
                break;
            case BACK:
                updateScreen = true;
                page = MAIN_MENU;
                break;
            default:
                break;
        }

    if (updateScreen)
        renderConfigMenuScreen();
}
