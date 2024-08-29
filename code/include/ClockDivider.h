#ifndef CLOCK_DIVIDER_H
#define CLOCK_DIVIDER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern int8_t numChannels;
extern const uint8_t outputPins[];

extern uint8_t bpm;

extern uint8_t stepCount;
extern bool resetIn;
extern bool oldResetIn;
extern bool isPause;

extern int8_t enc;
extern Adafruit_SSD1306 display;

enum DivEncoderOptions
{
    DIV_ENC_PLAY,
    DIV_ENC_MODE,
    DIV_ENC_RESET,
    DIV_ENC_BACK
};

extern uint8_t intClock;
extern bool clkMode;
extern bool outMode;
extern uint8_t page;

extern uint8_t divMode;
extern uint8_t resetMode;
extern int8_t newPosition;
extern int8_t oldPosition;

extern const char *const divModeOptions[];

char divBuffer[5];

extern const char playText[];
extern const char pauseText[];
extern const char backText[];

extern const char *const resetOptions[];

extern bool updateTrigger;
extern bool updateScreen;

enum divModeOptions
{
    MODE_2,
    MODE_NOT_2,
    MODE_PRIME,
    MODE_FIBO,
    MODE_ODD_EVEN
};

bool isPrime(int num)
{
    if (num <= 1)
        return false;
    for (int i = 2; i <= sqrt(num); i++)
    {
        if (num % i == 0)
            return false;
    }
    return true;
}

void calculateFibonacci(int n, int *fibonacciSequence)
{
    fibonacciSequence[0] = 1;
    fibonacciSequence[1] = 1;
    for (int i = 2; i < n; i++)
    {
        fibonacciSequence[i] = fibonacciSequence[i - 1] + fibonacciSequence[i - 2];
    }
}

void oledClockDivider()
{
    display.clearDisplay();
    int fibonacciSequence[numChannels];
    calculateFibonacci(numChannels, fibonacciSequence);

    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 48);
    display.print(stepCount + 1);
    display.print("S");

    display.setCursor(32, 48);
    display.setTextColor((enc == DIV_ENC_MODE) ? BLACK : WHITE, (enc == DIV_ENC_MODE) ? WHITE : BLACK);
    strcpy_P(divBuffer, (char *)pgm_read_word(&(divModeOptions[divMode])));
    display.print(divBuffer);

    display.setCursor(64, 48);
    display.setTextColor(WHITE, BLACK);
    display.print("--");

    display.setCursor(96, 48);
    display.setTextColor((enc == DIV_ENC_PLAY) ? BLACK : WHITE, (enc == DIV_ENC_PLAY) ? WHITE : BLACK);
    strcpy_P(divBuffer, isPause ? pauseText : playText);
    display.print(divBuffer);

    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 57);
    display.print(msDelay < 0 ? msDelay * -1 : msDelay);
    display.setCursor(18, 57);
    display.print("D");

    display.setCursor(32, 57);
    display.print(clkMode ? intClock : bpm);
    display.print("B");

    display.setCursor(64, 57);
    display.setTextColor((enc == DIV_ENC_RESET) ? BLACK : WHITE, (enc == DIV_ENC_RESET) ? WHITE : BLACK);
    strcpy_P(divBuffer, (char *)pgm_read_word(&(resetOptions[resetMode])));
    display.print(divBuffer);

    display.setCursor(96, 57);
    display.setTextColor((enc == DIV_ENC_BACK) ? BLACK : WHITE, (enc == DIV_ENC_BACK) ? WHITE : BLACK);
    strcpy_P(divBuffer, backText);
    display.print(divBuffer);

    for (int i = 0; i < numChannels; i++)
    {
        int x = (i >= 3) ? (i - 3) * 22 + 40 : i * 22 + 40;
        int y = (i >= 3) ? 32 : 10;

        bool shouldFill = false;
        switch (divMode)
        {
        case MODE_2:
            shouldFill = (stepCount % (1 << i) == 0);
            break;
        case MODE_NOT_2:
            shouldFill = (stepCount % (1 << i) != 0);
            break;
        case MODE_PRIME:
            shouldFill = (isPrime(i + 1) && stepCount % (i + 1) == 0);
            break;
        case MODE_FIBO:
            shouldFill = (stepCount % fibonacciSequence[i] == 0);
            break;
        case MODE_ODD_EVEN:
            shouldFill = (stepCount % 2 == 0 && i % 2 == 0) || (stepCount % 2 != 0 && i % 2 != 0);
            break;
        }

        if (shouldFill)
            display.fillCircle(x, y, 9, WHITE);
        else
            display.drawCircle(x, y, 9, WHITE);
    }
    display.display();
}

void clockDividerLoop()
{
    updateScreen = false;

    if (msDelay < 0)
        delay(msDelay * -1);

    oldResetIn = resetIn;
    oldButton = button;

    int fibonacciSequence[numChannels];
    calculateFibonacci(numChannels, fibonacciSequence);

    if (outMode)
        for (int i = 0; i < numChannels; i++)
        {
            digitalWrite(outputPins[i], LOW);
        }

    if (enc < 0)
        enc = DIV_ENC_BACK;
    else if (enc > DIV_ENC_BACK)
        enc = 0;

    if (buttonOn)
    {
        switch (enc)
        {
        case DIV_ENC_PLAY:
            isPause = !isPause;
            break;
        case DIV_ENC_MODE:
            divMode = divMode > 3 ? 0 : divMode + 1;
            break;
        case DIV_ENC_RESET:
            if (resetMode == 3)
                resetMode = 0;
            else
                resetMode++;
            break;
        case DIV_ENC_BACK:
            page = 0;
            updateScreen = true;
            break;
        }
    }

    if (resetIn && !oldResetIn)
    {
        stepCount = 0;
        oldResetIn = resetIn;
    }

    if (stepCount == 64)
        stepCount = 0;

    oledClockDivider();

    if (updateTrigger)
        for (int i = 0; i < numChannels; i++)
        {
            int value = LOW;
            switch (divMode)
            {
            case MODE_2:
                value = (stepCount % (1 << i) == 0) ? HIGH : LOW;
                break;
            case MODE_NOT_2:
                value = (stepCount % (1 << i) != 0) ? HIGH : LOW;
                break;
            case MODE_PRIME:
                value = (isPrime(i + 1) && stepCount % (i + 1) == 0) ? HIGH : LOW;
                break;
            case MODE_FIBO:
                value = (stepCount % fibonacciSequence[i] == 0) ? HIGH : LOW;
                break;
            case MODE_ODD_EVEN:
                value = ((stepCount % 2 == 0 && i % 2 == 0) || (stepCount % 2 != 0 && i % 2 != 0)) ? HIGH : LOW;
                break;
            }
            digitalWrite(outputPins[i], value);
        }
    delay(30);
    if (msDelay > 0)
        delay(msDelay);
}

#endif
