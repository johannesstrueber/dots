#ifndef MAINMENU_H
#define MAINMENU_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

void mainMenuOled()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(WHITE, BLACK);
    strcpy_P(headerBuffer, headerText);
    display.print(headerBuffer);
    for (int i = 0; i < 5; i++)
    {
        display.setCursor(0, 14 + i * 10);
        display.setTextColor((enc == i) ? BLACK : WHITE, (enc == i) ? WHITE : BLACK);
        strcpy_P(buffer, (char *)pgm_read_word(&(mainMenuOptions[i])));
        display.print(buffer);
    }
    display.display();
}

void mainMenuLoop()
{
    for (int i = 0; i < numChannels; i++)
        digitalWrite(outputPins[i], LOW);

    enc = enc > 4 ? 0 : (enc < 0 ? 4 : enc);

    updateScreen = false;
    if (buttonOn)
    {
        page = enc + 1;
        updateScreen = true;
    }

    mainMenuOled();
}

#endif