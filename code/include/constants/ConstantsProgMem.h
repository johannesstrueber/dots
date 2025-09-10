#ifndef CONSTANTS_PROG_MEM_H
#define CONSTANTS_PROG_MEM_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// main menu
const char headerText[] PROGMEM = "SELECT MODE";

const char mainMenuOption1[] PROGMEM = "1. STEP SEQ";
const char mainMenuOption2[] PROGMEM = "2. EUCLIDEAN SEQ";
const char mainMenuOption3[] PROGMEM = "3. RANDOM TRIGGER";
const char mainMenuOption4[] PROGMEM = "4. CLOCK DIVIDER";
const char mainMenuOption5[] PROGMEM = "5. CONFIG";
const char mainMenuOption6[] PROGMEM = "6. INSTRUCTIONS";

const char *const mainMenuOptions[] PROGMEM = {mainMenuOption1, mainMenuOption2, mainMenuOption3, mainMenuOption4, mainMenuOption5, mainMenuOption6};

// multi usage
const char resetOption1[] PROGMEM = "RST";
const char resetOption2[] PROGMEM = "NONE";
const char resetOption3[] PROGMEM = "S_RDM";
const char resetOption4[] PROGMEM = "S_BCK";
const char resetOption5[] PROGMEM = "B_RDM";
const char resetOption6[] PROGMEM = "P_RDM";

const char stepIndexText[] PROGMEM = "S";
const char delayIndexText[] PROGMEM = "D";
const char bpmIndexText[] PROGMEM = "B";
const char channelIndexText[] PROGMEM = "CH";
const char lengthIndexText[] PROGMEM = "L";
const char pagesIndexText[] PROGMEM = "P";
const char slashText[] PROGMEM = "/";
const char unselectedText[] PROGMEM = "--";

const char playText[] PROGMEM = "PLAY";
const char pauseText[] PROGMEM = "PAUSE";
const char backText[] PROGMEM = "BACK";
const char saveText[] PROGMEM = "SAVE";

const char *const resetOptions[] PROGMEM = {resetOption1, resetOption2, resetOption3, resetOption4, resetOption5, resetOption6};

// config menu
const char configMenuOption0[] PROGMEM = "CLK";
const char configMenuOption1[] PROGMEM = "BPM";
const char configMenuOption2[] PROGMEM = "OUT";
const char configMenuOption3[] PROGMEM = "BOOT";
const char configMenuOption4[] PROGMEM = "SAVE";
const char configMenuOption5[] PROGMEM = "BACK";

const char *const configMenuOptions[] PROGMEM = {configMenuOption0, configMenuOption1, configMenuOption2, configMenuOption3, configMenuOption4, configMenuOption5};

const char clkOptionInt[] PROGMEM = "INT";
const char clkOptionExt[] PROGMEM = "EXT";

const char outOptionTrig[] PROGMEM = "TRIG";
const char outOptionGate[] PROGMEM = "GATE";

const char bootOption1[] PROGMEM = "START";
const char bootOption2[] PROGMEM = "SEQ";
const char bootOption3[] PROGMEM = "RND";
const char bootOption4[] PROGMEM = "DIV";

const char *const bootOptions[] PROGMEM = {bootOption1, bootOption2, bootOption3, bootOption4};

// step sequencer
const char lengthText[] PROGMEM = "LEN";
const char offsetText[] PROGMEM = "OFF";

// euclidean sequencer
const char eucHitsText[] PROGMEM = "H";
const char eucRotationText[] PROGMEM = "R";
const char eucMuteText[] PROGMEM = "MUTE";
const char eucLiveText[] PROGMEM = "LIVE";

// random trigger
const char ranModeOption1[] PROGMEM = "ALL";
const char ranModeOption2[] PROGMEM = "SEQ";

const char *const ranModeOptions[] PROGMEM = {
    ranModeOption1,
    ranModeOption2,
};

#endif