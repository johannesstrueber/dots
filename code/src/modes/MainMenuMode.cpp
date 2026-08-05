#include "modes/MainMenuMode.h"

#include "State.h"
#include "UiStrings.h"
#include "utilities/DisplayUtils.h"
#include "utilities/EncoderUtils.h"
#include "utilities/TriggerUtils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

void renderMainMenuScreen() {
    static char buffer[18];
    static char headerBuffer[12];

    DisplayUtils::initDisplay();
    display.setCursor(0, 0);
    strcpy_P(headerBuffer, headerText);
    display.print(headerBuffer);

    for (int i = 0; i < 6; i++) {
        DisplayUtils::drawMenuItemFromArray(0, 14 + i * 8, mainMenuOptions, i, enc == i, buffer);
    }
    display.display();
}

void updateMainMenu() {
    TriggerUtils::setAllOutputsLow();

    EncoderUtils::handleEncoderBounds(enc, 0, 5);

    updateScreen = false;
    if (buttonOn) {
        page = static_cast<AppPage>(enc + 1);
        updateScreen = true;
    }

    renderMainMenuScreen();
}
