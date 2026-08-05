#include "State.h"

#include "modes/ClockDividerMode.h"
#include "modes/ConfigMenuMode.h"
#include "modes/EuclideanSequencerMode.h"
#include "modes/InstructionsMode.h"
#include "modes/MainMenuMode.h"
#include "modes/RandomTriggerMode.h"
#include "modes/StepSequencerMode.h"
#include "utilities/ResetHandler.h"
#include "utilities/SplashScreen.h"
#include "utilities/Startup.h"
#include "utilities/TriggerUtils.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <Wire.h>
#include <avr/pgmspace.h>

void setup() {
    pinMode(A0, INPUT);       // POT
    pinMode(4, INPUT_PULLUP); // BUTTON
    pinMode(5, OUTPUT);       // CH1
    pinMode(6, OUTPUT);       // CH2
    pinMode(7, OUTPUT);       // CH3
    pinMode(8, OUTPUT);       // CH4
    pinMode(9, OUTPUT);       // CH5
    pinMode(10, OUTPUT);      // CH6

    EEPROM.begin();

    // config settings
    clkMode = EEPROM.read(390);
    intClock = EEPROM.read(391);
    bootMode = EEPROM.read(392);
    outMode = EEPROM.read(393);
    resetMode = EEPROM.read(387);

    // program settings
    loadStepSequencerSettings();
    loadRandomTriggerSettings();
    loadEuclideanSequencerSettings();
    loadClockDividerSettings();

    if (seqCurrentPage > pages)
        seqCurrentPage = 1;
    if (seqCurrentLength > pages)
        seqCurrentLength = 1;
    if (seqCurrentOffset > pages)
        seqCurrentOffset = 0;
    if (resetMode > SEQ_PAGE_RANDOM)
        resetMode = RESET;
    if (intClock > 200)
        intClock = 80;
    if (bootMode > 4)
        bootMode = 0;
    if (ranMode > 1)
        ranMode = 0;
    if (ranActiveChannels > 6)
        ranActiveChannels = 6;

    // euclidean sequencer
    if (eucPatternLength < 1 || eucPatternLength > 32)
        eucPatternLength = 16;

    for (int i = 0; i < 6; i++) {
        if (eucTracks[i].steps < 1 || eucTracks[i].steps > 32)
            eucTracks[i].steps = 16;
        if (eucTracks[i].hits > eucTracks[i].steps)
            eucTracks[i].hits = eucTracks[i].steps / 2;
        if (eucTracks[i].rotation >= eucTracks[i].steps)
            eucTracks[i].rotation = 0;
    }
    if (ranActiveChannels < 1)
        ranActiveChannels = 1;

    page = static_cast<AppPage>(bootMode);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    showSplashScreen();
    startupBlink();

    updateEuclideanPatterns();
}

void loop() {
    oldClockIn = clockIn;
    clockIn = digitalRead(12);
    oldResetIn = resetIn;
    resetIn = digitalRead(11);
    oldButton = button;
    button = 1 - digitalRead(4);
    newPosition = Enc.read();

    if (outMode || isPause)
        TriggerUtils::setAllOutputsLow();

    if (!oldButton && button) {
        buttonOn = true;
        updateScreen = true;
    } else
        buttonOn = false;

    if (!isPause && page != MAIN_MENU) {
        if (!oldClockIn && clockIn && !clkMode) {
            beatAtMs[3] = beatAtMs[2];
            beatAtMs[2] = beatAtMs[1];
            beatAtMs[1] = beatAtMs[0];
            beatAtMs[0] = millis();
            bpm = 45000 / (beatAtMs[0] - beatAtMs[3]);
            stepCount++;
            updateScreen = true;
            updateTrigger = true;
        }

        else if (clkMode) {
            unsigned long currentTime = millis();
            unsigned long stepInterval = 15000 / intClock;

            if (currentTime - lastInternalClockStep >= stepInterval) {
                stepCount++;
                updateScreen = true;
                updateTrigger = true;
                lastInternalClockStep = currentTime;
            }
        }
        potIn = analogRead(A0);
        randomValuePotIn = random(-100, 100);
        uint8_t delayPercent = map(potIn, 0, 1023, 0, 100);
        msDelay = round(randomValuePotIn * delayPercent / 100 * 15 / (clkMode ? intClock : bpm));
    }

    if (newPosition + 2 < oldPosition) {
        oldPosition = newPosition;
        if (!encLock)
            enc--;
        if (isPause || (page != SEQUENCER && page != EUCLIDEAN_SEQUENCER))
            updateScreen = true;
        else
            needsThrottledUpdate = true;

        if (!clkMode)
            updateTrigger = false;
    } else if (newPosition - 2 > oldPosition) {
        oldPosition = newPosition;
        if (!encLock)
            enc++;
        if (isPause || (page != SEQUENCER && page != EUCLIDEAN_SEQUENCER))
            updateScreen = true;
        else
            needsThrottledUpdate = true;

        if (!clkMode)
            updateTrigger = false;
    }

    handleReset();

    if (needsThrottledUpdate && (millis() - lastScreenUpdate >= SCREEN_UPDATE_INTERVAL)) {
        updateScreen = true;
        needsThrottledUpdate = false;
        lastScreenUpdate = millis();
    }

    if (updateScreen) {
        lastScreenUpdate = millis();
        switch (page) {
            case MAIN_MENU:
                updateMainMenu();
                break;
            case SEQUENCER:
                updateStepSequencer();
                break;
            case EUCLIDEAN_SEQUENCER:
                updateEuclideanSequencer();
                break;
            case RANDOM_TRIGGER:
                updateRandomTrigger();
                break;
            case CLOCK_DIVIDER:
                updateClockDivider();
                break;
            case CONFIG:
                updateConfigMenu();
                break;
            case INSTRUCTIONS:
                updateInstructions();
                break;
        }
    }
}
