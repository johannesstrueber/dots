#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

extern Adafruit_SSD1306 display;

class DisplayUtils {
  public:
    static void drawMenuSeparator() {
        display.drawFastHLine(0, 45, 128, WHITE);
    }

    static void drawMenuItem(uint8_t x, uint8_t y, const char *text, bool isSelected) {
        display.setCursor(x, y);
        display.setTextColor(isSelected ? BLACK : WHITE, isSelected ? WHITE : BLACK);
        display.print(text);
        display.setTextColor(WHITE, BLACK); // Reset to default
    }

    static void drawDelayDisplay(uint8_t x, uint8_t y, int8_t msDelay) {
        display.setCursor(x, y);
        display.setTextColor(WHITE, BLACK);
        display.print(msDelay < 0 ? msDelay * -1 : msDelay);
        display.setCursor(x + 18, y);
        display.print("D");
    }

    static void drawBPMDisplay(uint8_t x, uint8_t y, uint8_t bpm, uint8_t intClock, bool clkMode) {
        display.setCursor(x, y);
        display.setTextColor(WHITE, BLACK);
        display.print(clkMode ? intClock : bpm);
        display.print("B");
    }

    static void initDisplay() {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
    }

    static void drawStepCounter(uint8_t x, uint8_t y, uint8_t stepCount) {
        display.setCursor(x, y);
        display.setTextColor(WHITE, BLACK);
        display.print(stepCount + 1);
        display.print("S");
    }

    static void drawMenuItemProgMem(uint8_t x, uint8_t y, const char *progmemText, bool isSelected, char *buffer) {
        strcpy_P(buffer, progmemText);
        drawMenuItem(x, y, buffer, isSelected);
    }

    static void drawMenuItemFromArray(uint8_t x, uint8_t y, const char *const *progmemArray, uint8_t index, bool isSelected, char *buffer) {
        strcpy_P(buffer, (char *)pgm_read_word(&(progmemArray[index])));
        drawMenuItem(x, y, buffer, isSelected);
    }
};

#endif
