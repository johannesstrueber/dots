#ifndef UI_STRINGS_H
#define UI_STRINGS_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// main menu
extern const char headerText[] PROGMEM;

extern const char mainMenuOption1[] PROGMEM;
extern const char mainMenuOption2[] PROGMEM;
extern const char mainMenuOption3[] PROGMEM;
extern const char mainMenuOption4[] PROGMEM;
extern const char mainMenuOption5[] PROGMEM;
extern const char mainMenuOption6[] PROGMEM;

extern const char *const mainMenuOptions[] PROGMEM;

// multi usage
extern const char resetOption1[] PROGMEM;
extern const char resetOption2[] PROGMEM;
extern const char resetOption3[] PROGMEM;
extern const char resetOption4[] PROGMEM;
extern const char resetOption5[] PROGMEM;
extern const char resetOption6[] PROGMEM;

extern const char stepIndexText[] PROGMEM;
extern const char delayIndexText[] PROGMEM;
extern const char bpmIndexText[] PROGMEM;
extern const char channelIndexText[] PROGMEM;
extern const char lengthIndexText[] PROGMEM;
extern const char pagesIndexText[] PROGMEM;
extern const char slashText[] PROGMEM;
extern const char unselectedText[] PROGMEM;

extern const char playText[] PROGMEM;
extern const char pauseText[] PROGMEM;
extern const char backText[] PROGMEM;
extern const char saveText[] PROGMEM;

extern const char *const resetOptions[] PROGMEM;

// config menu
extern const char configMenuOption0[] PROGMEM;
extern const char configMenuOption1[] PROGMEM;
extern const char configMenuOption2[] PROGMEM;
extern const char configMenuOption3[] PROGMEM;
extern const char configMenuOption4[] PROGMEM;
extern const char configMenuOption5[] PROGMEM;

extern const char *const configMenuOptions[] PROGMEM;

extern const char clkOptionInt[] PROGMEM;
extern const char clkOptionExt[] PROGMEM;

extern const char outOptionTrig[] PROGMEM;
extern const char outOptionGate[] PROGMEM;

extern const char bootOption1[] PROGMEM;
extern const char bootOption2[] PROGMEM;
extern const char bootOption3[] PROGMEM;
extern const char bootOption4[] PROGMEM;
extern const char bootOption5[] PROGMEM;

extern const char *const bootOptions[] PROGMEM;

// step sequencer
extern const char lengthText[] PROGMEM;
extern const char offsetText[] PROGMEM;

// euclidean sequencer
extern const char eucHitsText[] PROGMEM;
extern const char eucRotationText[] PROGMEM;
extern const char eucMuteText[] PROGMEM;
extern const char eucLiveText[] PROGMEM;

// random trigger
extern const char ranModeOption1[] PROGMEM;
extern const char ranModeOption2[] PROGMEM;

extern const char *const ranModeOptions[] PROGMEM;

#endif
