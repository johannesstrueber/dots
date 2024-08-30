#ifndef RANDOM_TRIGGER_H
#define RANDOM_TRIGGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

char ranBuffer[5];

enum RanEncoderOptions
{
    RAN_ENC_CHANNELS = 7,
    RAN_ENC_MODE,
    RAN_ENC_PLAY,
    RAN_ENC_RESET,
    RAN_ENC_BACK
};

extern const char *const resetOptions[];

enum RanModeOptions
{
    RAN_MODE_ALL,
    RAN_MODE_SEQ,
    RAN_MODE_STEP
};

extern const char *const ranModeOptions[];

extern uint8_t randomChannelValues[6];
extern uint8_t ranActiveChannels;

void oledRan()
{

    display.clearDisplay();
    for (int i = 0; i < ranActiveChannels; i++)
    {
        int x = 20 * i + 11;
        display.fillCircle(x, 36 - randomChannelValues[i] * 3, 3, WHITE);

        if (i == enc && !encLock)
            display.drawRoundRect(x - 4, 0, 9, 36, 6, WHITE);
        else
            display.drawFastVLine(x, 0, 36, WHITE);
    }

    if (ranMode == RAN_MODE_SEQ)
        display.fillRect(20 * stepCount * 1 + 9, 43, 5, 2, WHITE);

    display.setCursor(0, 48);
    display.setTextColor(WHITE, BLACK);
    display.print(stepCount + 1);
    display.print("S");

    display.setCursor(32, 48);
    display.setTextColor((enc == RAN_ENC_CHANNELS) ? BLACK : WHITE, (enc == RAN_ENC_CHANNELS) ? WHITE : BLACK);
    display.print(ranActiveChannels);
    display.print("C");

    display.setCursor(64, 48);

    display.setTextColor((enc == RAN_ENC_MODE) ? BLACK : WHITE, (enc == RAN_ENC_MODE) ? WHITE : BLACK);
    strcpy_P(ranBuffer, (char *)pgm_read_word(&(ranModeOptions[ranMode])));
    display.print(ranBuffer);

    display.setCursor(96, 48);
    display.setTextColor((enc == RAN_ENC_PLAY) ? BLACK : WHITE, (enc == RAN_ENC_PLAY) ? WHITE : BLACK);
    strcpy_P(ranBuffer, isPause ? pauseText : playText);
    display.print(ranBuffer);

    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 57);
    display.print(msDelay < 0 ? msDelay * -1 : msDelay);
    display.setCursor(18, 57);
    display.print("D");

    display.setCursor(32, 57);
    display.print(clkMode ? intClock : bpm);
    display.print("B");

    display.setCursor(64, 57);
    display.setTextColor((enc == RAN_ENC_RESET) ? BLACK : WHITE, (enc == RAN_ENC_RESET) ? WHITE : BLACK);
    strcpy_P(ranBuffer, (char *)pgm_read_word(&(resetOptions[resetMode])));
    display.print(ranBuffer);

    display.setCursor(96, 57);
    display.setTextColor((enc == RAN_ENC_BACK) ? BLACK : WHITE, (enc == RAN_ENC_BACK) ? WHITE : BLACK);
    strcpy_P(ranBuffer, backText);
    display.print(ranBuffer);

    display.display();
};

void randomLoop()
{
    int8_t newPosition = Enc.read();
    static int8_t oldPosition = -2;
    updateScreen = false;

    if (msDelay < 0)
        delay(msDelay * -1);

    if (outMode)
        for (int i = 0; i < numChannels; i++)
            digitalWrite(outputPins[i], LOW);

    if (stepCount > ranActiveChannels - 1)
        stepCount = 0;

    if (enc > RAN_ENC_BACK)
        enc = 0;
    else if (enc < 0)
        enc = RAN_ENC_BACK;

    if (encLock)
    {
        if (newPosition < oldPosition && randomChannelValues[enc] > 0)
            randomChannelValues[enc]--;
        else if (newPosition > oldPosition && randomChannelValues[enc] < 12)
            randomChannelValues[enc]++;
        oldPosition = newPosition;
    }

    if (buttonOn)
        switch (enc)
        {
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
            isPause = !isPause;
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
            encLock = !encLock;
            break;
        }

    oledRan();
    if (updateTrigger)
        for (int i = 0; i < ranActiveChannels; i++)
        {
            int trigger = random(0, 12) < randomChannelValues[i];
            switch (ranMode)
            {
            case RAN_MODE_ALL:
                if (trigger)
                    digitalWrite(outputPins[i], HIGH);
                else
                    digitalWrite(outputPins[i], LOW);
                break;
            case RAN_MODE_SEQ:
                if (trigger && i == stepCount)
                    digitalWrite(outputPins[i], HIGH);
                else
                    digitalWrite(outputPins[i], LOW);
                break;
            }
        }

    delay(30);
    if (msDelay > 0)
        delay(msDelay);
}

#endif