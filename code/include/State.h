#ifndef STATE_H
#define STATE_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Encoder.h>

// Top-level UI pages. bootMode (persisted in EEPROM) and the encoder-driven
// main menu selection both cross into this type via static_cast<AppPage>(...).
enum AppPage : uint8_t { MAIN_MENU, SEQUENCER, EUCLIDEAN_SEQUENCER, RANDOM_TRIGGER, CLOCK_DIVIDER, CONFIG, INSTRUCTIONS };

// Shared reset behavior, selectable from every mode's menu (with per-mode caps).
enum ResetModeOptions { RESET, NONE, STEP_RANDOM, STEP_BACK, SEQ_BAR_RANDOM, SEQ_PAGE_RANDOM };

// Hardware
extern const uint8_t outputPins[];
extern int8_t numChannels;
extern Adafruit_SSD1306 display;
extern Encoder Enc;

// Clock / reset inputs
extern bool clockIn;
extern bool oldClockIn;
extern bool resetIn;
extern bool oldResetIn;

// Encoder + button state
extern int8_t enc;
extern bool encLock;
extern int8_t newPosition;
extern int8_t oldPosition;
extern bool button;
extern bool oldButton;
extern bool buttonOn;

// Step sequencer matrix geometry
extern const uint8_t rows;
extern const uint8_t cols;
extern const uint8_t pages;
extern const uint8_t barLength;
extern const uint8_t seqMatrixSize;
extern byte seqMatrix[];
extern uint8_t seqCurrentPage;
extern uint8_t seqCurrentLength;
extern uint8_t seqCurrentOffset;
extern uint8_t previousPage;

// Random trigger shared state
extern uint8_t randomChannelValues[6];
extern uint8_t ranMode;
extern uint8_t ranActiveChannels;

// Navigation / timing
extern AppPage page;
extern uint8_t bootMode;
extern uint8_t resetMode;
extern bool isPause;
extern uint8_t stepCount;

// Clock configuration
extern bool clkMode;
extern bool outMode;
extern uint8_t intClock;
extern uint8_t bpm;
extern uint16_t beatAtMs[4];
extern unsigned long lastInternalClockStep;

// Display refresh
extern bool updateScreen;
extern bool updateTrigger;
extern bool needsThrottledUpdate;
extern unsigned long lastScreenUpdate;
extern const unsigned long SCREEN_UPDATE_INTERVAL;

// Pot / random delay
extern int16_t potIn;
extern int8_t randomValuePotIn;
extern int8_t msDelay;

#endif
