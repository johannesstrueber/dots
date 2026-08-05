#include "State.h"

const uint8_t outputPins[] = {5, 6, 7, 8, 9, 10};
int8_t numChannels = 6;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Encoder Enc(3, 2);

bool clockIn = false;
bool oldClockIn = false;
bool resetIn = false;
bool oldResetIn = false;

int8_t enc = 0;
bool encLock = false;
int8_t newPosition = 2;
int8_t oldPosition = -2;
bool button = false;
bool oldButton = false;
bool buttonOn = false;

const uint8_t rows = 6;
const uint8_t cols = 16;
const uint8_t pages = 8;
const uint8_t barLength = 4;
const uint8_t seqMatrixSize = pages * rows * cols / 8;
byte seqMatrix[seqMatrixSize];
uint8_t seqCurrentPage = 1;
uint8_t seqCurrentLength = 1;
uint8_t seqCurrentOffset = 0;
uint8_t previousPage = 0;

uint8_t randomChannelValues[6] = {0, 0, 0, 0, 0, 0};
uint8_t ranMode = 0;
uint8_t ranActiveChannels = 6;

AppPage page = MAIN_MENU;
uint8_t bootMode = 0;
uint8_t resetMode = 0;
bool isPause = false;
uint8_t stepCount = 0;

bool clkMode = true;
bool outMode = true;
uint8_t intClock = 120;
uint8_t bpm;
uint16_t beatAtMs[4] = {0, 0, 0, 0};
unsigned long lastInternalClockStep = 0;

bool updateScreen = true;
bool updateTrigger = true;
bool needsThrottledUpdate = false;
unsigned long lastScreenUpdate = 0;
const unsigned long SCREEN_UPDATE_INTERVAL = 200;

int16_t potIn = 0;
int8_t randomValuePotIn = 0;
int8_t msDelay = 0;
