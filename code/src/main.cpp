#include "utils/SplashScreen.h"
#include "utils/Startup.h"
#include "utils/ConstantsProgMem.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

const uint8_t outputPins[] = {5, 6, 7, 8, 9, 10};
int8_t numChannels = 6;

bool clockIn = false;
bool oldClockIn = false;
bool resetIn = false;
bool oldResetIn = false;

Encoder Enc(3, 2);
int8_t enc = 0;
bool encLock = false;

const uint8_t rows = 6;
const uint8_t cols = 16;
const uint8_t pages = 8;
const uint8_t barLength = 4;

const uint8_t seqMatrixSize = pages * rows * cols / 8;
byte seqMatrix[seqMatrixSize];

uint8_t randomChannelValues[6] = {0, 0, 0, 0, 0, 0};

uint8_t seqCurrentPage = 1;
uint8_t seqCurrentLength = 1;
uint8_t seqCurrentOffset = 0;
uint8_t resetMode = 0;

uint8_t page = 0;
uint8_t bootMode = 0;

bool isPause = false;
uint8_t bpm;
uint16_t beatAtMs[4] = {0, 0, 0, 0};

bool clkMode = true;
bool outMode = true;
uint8_t intClock = 120;

uint8_t stepCount = 0;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

uint8_t divMode = 0;
uint8_t ranMode = 0;
uint8_t ranActiveChannels = 6;

enum MainPages
{
  MAIN_MENU,
  SEQUENCER,
  RANDOM_TRIGGER,
  CLOCK_DIVIDER,
  CONFIG,
  INSTRUCTIONS
};

int8_t newPosition = 2;
int8_t oldPosition = -2;

bool updateScreen = true;
bool updateTrigger = true;
bool button = false;
bool oldButton = false;
bool buttonOn = false;

int16_t potIn = 0;
int8_t randomValuePotIn = 0;
int8_t msDelay = 0;

#include "MainMenu.h"
#include "Sequencer.h"
#include "RandomTrigger.h"
#include "ClockDivider.h"
#include "ConfigMenu.h"
#include "Instructions.h"
#include "utils/HandleReset.h"

void setup()
{
  pinMode(A0, INPUT);       // POT
  pinMode(4, INPUT_PULLUP); // BUTTON
  pinMode(5, OUTPUT);       // CH1
  pinMode(6, OUTPUT);       // CH2
  pinMode(7, OUTPUT);       // CH3
  pinMode(8, OUTPUT);       // CH4
  pinMode(9, OUTPUT);       // CH5
  pinMode(10, OUTPUT);      // CH6

  EEPROM.begin();

  // settings
  clkMode = EEPROM.read(390);
  intClock = EEPROM.read(391);
  bootMode = EEPROM.read(392);
  outMode = EEPROM.read(393);
  resetMode = EEPROM.read(387);
  // sequencer
  for (int pa = 0; pa < pages; pa++)
    for (int ro = 0; ro < rows; ro++)
      for (int co = 0; co < cols; co++)
      {
        int index = pa * rows * cols + ro * cols + co;
        int safeByteIndex = (index - (index % 8)) / 8;
        if (safeByteIndex < seqMatrixSize)
        {
          if (EEPROM.read(safeByteIndex) & (1 << (index % 8)))
            setBit(pa, ro, co);
          else
            clearBit(pa, ro, co);
        }
      }
  seqCurrentPage = EEPROM.read(384);
  seqCurrentLength = EEPROM.read(385);
  seqCurrentOffset = EEPROM.read(386);
  // clock divider
  divMode = EEPROM.read(394);
  // ran trigger
  ranMode = EEPROM.read(395);
  ranActiveChannels = EEPROM.read(396);

  if (seqCurrentPage > pages)
    seqCurrentPage = 1;
  if (seqCurrentLength > pages)
    seqCurrentLength = 1;
  if (seqCurrentOffset > pages)
    seqCurrentOffset = 0;
  if (resetMode > 5)
    resetMode = 0;
  if (intClock > 200)
    intClock = 80;
  if (bootMode > 3)
    bootMode = 0;
  if (divMode > 4)
    divMode = 0;
  if (ranMode > 1)
    ranMode = 0;
  if (ranActiveChannels > 6)
    ranActiveChannels = 6;
  if (ranActiveChannels < 1)
    ranActiveChannels = 1;

  page = bootMode;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  splashScreen();
  startupBlink();
}

void loop()
{
  oldClockIn = clockIn;
  clockIn = digitalRead(12);
  oldResetIn = resetIn;
  resetIn = digitalRead(11);
  oldButton = button;
  button = 1 - digitalRead(4);
  newPosition = Enc.read();

  if (outMode || isPause)
    for (int i = 0; i < numChannels; i++)
      digitalWrite(outputPins[i], LOW);

  if (!oldButton && button)
  {
    buttonOn = true;
    updateScreen = true;
  }
  else
    buttonOn = false;

  if (!isPause && page != 0)
  {
    if (!oldClockIn && clockIn && !clkMode)
    {
      beatAtMs[3] = beatAtMs[2];
      beatAtMs[2] = beatAtMs[1];
      beatAtMs[1] = beatAtMs[0];
      beatAtMs[0] = millis();
      bpm = 45000 / (beatAtMs[0] - beatAtMs[3]);
      stepCount++;
      updateScreen = true;
      updateTrigger = true;
    }

    else if (clkMode)
    {
      delay(15000 / intClock);
      stepCount++;
      updateScreen = true;
      updateTrigger = true;
    }
    potIn = analogRead(A0);
    randomValuePotIn = random(-100, 100);
    uint8_t delayPercent = map(potIn, 0, 1023, 0, 100);
    msDelay = round(randomValuePotIn * delayPercent / 100 * 15 / (clkMode ? intClock : bpm));
  }

  if (newPosition + 2 < oldPosition)
  {
    oldPosition = newPosition;
    if (!encLock)
      enc--;
    updateScreen = true;
    updateTrigger = false;
  }
  else if (newPosition - 2 > oldPosition)
  {
    oldPosition = newPosition;
    if (!encLock)
      enc++;
    updateScreen = true;
    updateTrigger = false;
  }

  handleReset();

  if (updateScreen)
    switch (page)
    {
    case MAIN_MENU:
      mainMenuLoop();
      break;
    case SEQUENCER:
      sequencerLoop();
      break;
    case RANDOM_TRIGGER:
      randomLoop();
      break;
    case CLOCK_DIVIDER:
      clockDividerLoop();
      break;
    case CONFIG:
      configMenuLoop();
      break;
    case INSTRUCTIONS:
      instructionsLoop();
      break;
    }
}