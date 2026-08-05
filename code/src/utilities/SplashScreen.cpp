#include "utilities/SplashScreen.h"

#include "State.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

namespace {
const char splashScreenText[] PROGMEM = "DOTS 1.1";
}

void showSplashScreen() {
    char splashScreenTextBuffer[9];

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.fillCircle(10, 12, 9, WHITE);
    display.fillCircle(18, 39, 7, WHITE);
    display.fillCircle(110, 6, 5, WHITE);
    display.fillCircle(89, 26, 8, WHITE);
    display.fillCircle(61, 16, 13, WHITE);
    display.fillCircle(42, 48, 12, WHITE);
    display.setCursor(76, 57);
    strcpy_P(splashScreenTextBuffer, splashScreenText);
    display.print(splashScreenTextBuffer);
    display.display();
}
