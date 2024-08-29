#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

extern Adafruit_SSD1306 display;
extern uint8_t page;
extern bool buttonOn;

const unsigned char bitmapQRCode[] PROGMEM = {
    0x00, 0x03, 0x33, 0xf3, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x33, 0xf3, 0xf0, 0x00, 0x00, 0x3f, 0xf3,
    0xc3, 0x3f, 0x33, 0xff, 0x00, 0x3f, 0xf3, 0xc3, 0x3f, 0x33, 0xff, 0x00, 0x30, 0x33, 0x30, 0xf0,
    0xf3, 0x03, 0x00, 0x30, 0x33, 0x30, 0xf0, 0xf3, 0x03, 0x00, 0x30, 0x33, 0x33, 0x0f, 0xf3, 0x03,
    0x00, 0x30, 0x33, 0x33, 0x0f, 0xf3, 0x03, 0x00, 0x30, 0x33, 0xc0, 0x3f, 0xf3, 0x03, 0x00, 0x30,
    0x33, 0xc0, 0x3f, 0xf3, 0x03, 0x00, 0x3f, 0xf3, 0x0c, 0xfc, 0x33, 0xff, 0x00, 0x3f, 0xf3, 0x0c,
    0xfc, 0x33, 0xff, 0x00, 0x00, 0x03, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x03, 0x33, 0x33, 0x30,
    0x00, 0x00, 0xff, 0xff, 0x00, 0x0c, 0xff, 0xff, 0xc0, 0xff, 0xff, 0x00, 0x0c, 0xff, 0xff, 0xc0,
    0xcc, 0xc0, 0x0f, 0xf3, 0x00, 0xc3, 0x00, 0xcc, 0xc0, 0x0f, 0xf3, 0x00, 0xc3, 0x00, 0x3f, 0x0f,
    0xcc, 0x0f, 0x00, 0xff, 0x00, 0x3f, 0x0f, 0xcc, 0x0f, 0x00, 0xff, 0x00, 0xc3, 0x00, 0xc3, 0x03,
    0xcc, 0x3c, 0x00, 0xc3, 0x00, 0xc3, 0x03, 0xcc, 0x3c, 0x00, 0xcc, 0xfc, 0x0c, 0x0c, 0x0f, 0xff,
    0xc0, 0xcc, 0xfc, 0x0c, 0x0c, 0x0f, 0xff, 0xc0, 0xf0, 0x30, 0x30, 0x33, 0x03, 0xcc, 0x00, 0xf0,
    0x30, 0x30, 0x33, 0x03, 0xcc, 0x00, 0xf0, 0xcc, 0x0f, 0x33, 0xc0, 0xc3, 0x00, 0xf0, 0xcc, 0x0f,
    0x33, 0xc0, 0xc3, 0x00, 0x3c, 0x30, 0xc0, 0x3f, 0x0c, 0x33, 0x00, 0x3c, 0x30, 0xc0, 0x3f, 0x0c,
    0x33, 0x00, 0xc3, 0xcc, 0xcc, 0x03, 0x3f, 0x3c, 0xc0, 0xc3, 0xcc, 0xcc, 0x03, 0x3f, 0x3c, 0xc0,
    0x0f, 0x30, 0xcc, 0x30, 0x00, 0x03, 0xc0, 0x0f, 0x30, 0xcc, 0x30, 0x00, 0x03, 0xc0, 0xff, 0xff,
    0x3f, 0x30, 0x3f, 0x0f, 0x00, 0xff, 0xff, 0x3f, 0x30, 0x3f, 0x0f, 0x00, 0x00, 0x03, 0x0f, 0x30,
    0x33, 0x0c, 0x00, 0x00, 0x03, 0x0f, 0x30, 0x33, 0x0c, 0x00, 0x3f, 0xf3, 0x03, 0xfc, 0x3f, 0x00,
    0xc0, 0x3f, 0xf3, 0x03, 0xfc, 0x3f, 0x00, 0xc0, 0x30, 0x33, 0xf3, 0xcf, 0x00, 0x0c, 0x00, 0x30,
    0x33, 0xf3, 0xcf, 0x00, 0x0c, 0x00, 0x30, 0x33, 0x30, 0xcc, 0xcc, 0x03, 0xc0, 0x30, 0x33, 0x30,
    0xcc, 0xcc, 0x03, 0xc0, 0x30, 0x33, 0xcc, 0xf0, 0x3c, 0x33, 0x00, 0x30, 0x33, 0xcc, 0xf0, 0x3c,
    0x33, 0x00, 0x3f, 0xf3, 0x03, 0x0c, 0x03, 0xcf, 0xc0, 0x3f, 0xf3, 0x03, 0x0c, 0x03, 0xcf, 0xc0,
    0x00, 0x03, 0xf3, 0xf0, 0x00, 0xfc, 0x00, 0x00, 0x03, 0xf3, 0xf0, 0x00, 0xfc, 0x00};

void instructionsLoop()
{

    if (buttonOn)
        page = 0;

    display.setCursor(0, 0);
    display.clearDisplay();
    display.drawBitmap(39, 7, bitmapQRCode, 50, 50, 1);
    display.display();
}

#endif