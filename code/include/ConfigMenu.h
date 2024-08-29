#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H

#include <EEPROM.h>
#include <Arduino.h>
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Encoder Enc;
extern int8_t enc;

extern Adafruit_SSD1306 display;
int8_t configMenuOptionCount = 6;

enum ConfigOptions
{
    CLK,
    BPM,
    OUT,
    BOOT,
    SAVE,
    BACK
};

extern const char clkOptionInt[];
extern const char clkOptionExt[];

extern const char outOptionTrig[];
extern const char outOptionGate[];

extern const char *const bootOptions[];

extern bool clkMode;
extern bool outMode;
extern uint8_t bootMode;
extern uint8_t page;
extern uint8_t intClock;
extern uint8_t bpm;

extern bool encLock;
extern uint8_t divMode;
extern uint8_t ranMode;
extern uint8_t ranActiveChannels;

extern const uint8_t seqMatrixSize;
extern byte seqMatrix[seqMatrixSize];

extern uint8_t seqCurrentOffset;
extern uint8_t seqCurrentLength;
extern uint8_t seqCurrentPage;
extern uint8_t resetMode;
extern bool isPause;

extern bool updateScreen;
extern bool buttonOn;

void oledConfigMenu()
{
    display.clearDisplay();
    for (int i = 0; i < configMenuOptionCount; i++)
    {
        display.setCursor(0, i * 9);
        char configBuffer[5];

        display.setTextColor((enc == i) ? BLACK : WHITE, (enc == i) ? WHITE : BLACK);
        strcpy_P(configBuffer, (char *)pgm_read_word(&(configMenuOptions[i])));
        display.print(configBuffer);
    }
    char configBuffer[5];
    display.setTextColor(WHITE);
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

void configMenuLoop()
{
    int8_t newPosition = Enc.read();
    static int8_t oldPosition = -2;

    if (encLock)
    {
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

    if (enc > configMenuOptionCount)
        enc = 0;

    else if (enc < 0)
        enc = configMenuOptionCount - 1;

    if (buttonOn)
        switch (enc)
        {
        case CLK:
            clkMode = !clkMode;
            break;
        case BPM:
            encLock = !encLock;
            enc = 1;
            break;
        case OUT:
            outMode = !outMode;
            break;
        case BOOT:
            bootMode++;
            if (bootMode > 3)
                bootMode = 0;
            break;
        case SAVE:
            for (int i = 0; i < seqMatrixSize; i++)
            {
                byte EEPROMbyte = seqMatrix[i];
                EEPROM.write(i, EEPROMbyte);
            }
            EEPROM.write(384, seqCurrentPage);
            EEPROM.write(385, seqCurrentLength);
            EEPROM.write(386, seqCurrentOffset);
            EEPROM.write(387, resetMode);
            EEPROM.write(388, isPause);
            EEPROM.write(390, clkMode);
            EEPROM.write(391, intClock);
            EEPROM.write(392, bootMode);
            EEPROM.write(393, outMode);
            EEPROM.write(394, divMode);
            EEPROM.write(395, ranMode);
            EEPROM.write(396, ranActiveChannels);
            break;
        case BACK:
            updateScreen = true;
            page = 0;
            break;
        default:
            break;
        }

    if (updateScreen)
        oledConfigMenu();
}

#endif