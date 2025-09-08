#ifndef MAINMENU_H
#define MAINMENU_H

#include "utilities/Display.h"
#include "utilities/Encoder.h"
#include "utilities/MenuLayout.h"
#include "utilities/Trigger.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

char buffer[18];
char headerBuffer[12];

extern Adafruit_SSD1306 display;
extern int8_t enc;
extern bool buttonOn;
extern uint8_t page;

extern bool updateScreen;

extern int8_t numChannels;
extern const uint8_t outputPins[];

extern const char *const mainMenuOptions[];
extern const char headerText[];

extern const char *const configMenuOptions[];

void mainMenuOled() {
    DisplayUtils::initDisplay();
    display.setCursor(0, 0);
    strcpy_P(headerBuffer, headerText);
    display.print(headerBuffer);

    for (int i = 0; i < 6; i++) {
        DisplayUtils::drawMenuItemFromArray(0, 14 + i * 8, mainMenuOptions, i, enc == i, buffer);
    }
    display.display();
}

void mainMenuLoop() {
    TriggerUtils::setAllOutputsLow();

    EncoderUtils::handleEncoderBounds(enc, 0, 5);

    updateScreen = false;
    if (buttonOn) {
        page = enc + 1;
        updateScreen = true;
    }

    mainMenuOled();
}

#endif