#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

extern Adafruit_SSD1306 display;
extern const char stepIndexText[];
extern const char delayIndexText[];
extern const char bpmIndexText[];
extern const char slashText[];
extern const char pagesIndexText[];

class DisplayUtils {
  private:
    static void setColors(bool isSelected) {
        display.setTextColor(isSelected ? BLACK : WHITE, isSelected ? WHITE : BLACK);
    }

    static void resetColors() {
        display.setTextColor(WHITE, BLACK);
    }

    static void drawMenuItem(uint8_t x, uint8_t y, const char *text, bool isSelected) {
        display.setCursor(x, y);
        setColors(isSelected);
        display.print(text);
        resetColors();
    }

  public:
    static void initDisplay() {
        display.clearDisplay();
        display.setTextSize(1);
        resetColors();
    }

    static void drawMenuSeparator() {
        display.drawFastHLine(0, 45, 128, WHITE);
    }

    static void drawMenuItemProgMem(uint8_t x, uint8_t y, const char *progmemText, bool isSelected, char *buffer) {
        strcpy_P(buffer, progmemText);
        drawMenuItem(x, y, buffer, isSelected);
    }

    static void drawMenuItemFromArray(uint8_t x, uint8_t y, const char *const *progmemArray, uint8_t index, bool isSelected, char *buffer) {
        strcpy_P(buffer, (char *)pgm_read_word(&(progmemArray[index])));
        drawMenuItem(x, y, buffer, isSelected);
    }

    static void drawNumberText(uint8_t x, uint8_t y, int number, const char *progmemText, bool isSelected, char *buffer) {
        display.setCursor(x, y);
        setColors(isSelected);
        display.print(number);
        strcpy_P(buffer, progmemText);
        display.print(buffer);
        resetColors();
    }

    static void drawDelay(uint8_t x, uint8_t y, int8_t msDelay, char *buffer) {
        char numStr[4];
        itoa(abs(msDelay), numStr, 10);
        drawMenuItem(x, y, numStr, false);

        strcpy_P(buffer, delayIndexText);
        drawMenuItem(x + 18, y, buffer, false);
    }

    static void drawBPM(uint8_t x, uint8_t y, uint8_t bpm, uint8_t intClock, bool clkMode, char *buffer) {
        char numStr[4];
        itoa(clkMode ? intClock : bpm, numStr, 10);
        drawMenuItem(x, y, numStr, false);

        strcpy_P(buffer, bpmIndexText);
        display.setCursor(x + strlen(numStr) * 6, y);
        display.print(buffer);
    }

    static void drawStepCounter(uint8_t x, uint8_t y, uint8_t stepCount, char *buffer) {
        display.setCursor(x, y);
        resetColors();
        display.print(stepCount + 1);
        strcpy_P(buffer, stepIndexText);
        display.print(buffer);
    }

    static void drawPages(uint8_t x, uint8_t y, uint8_t currentPage, uint8_t totalPages, bool isSelected, char *buffer) {
        display.setCursor(x, y);
        setColors(isSelected);
        display.print(currentPage);
        strcpy_P(buffer, slashText);
        display.print(buffer);
        display.print(totalPages);
        strcpy_P(buffer, pagesIndexText);
        display.print(buffer);
        resetColors();
    }
};

#endif
