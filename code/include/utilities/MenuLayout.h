#ifndef MENU_LAYOUT_H
#define MENU_LAYOUT_H

#include <Arduino.h>

class MenuLayout {
  public:
    // Standard positions for bottom menu items (2 rows)
    static const uint8_t MENU_Y_TOP = 48;
    static const uint8_t MENU_Y_BOTTOM = 57;
    static const uint8_t MENU_SEPARATOR_Y = 42;

    // Standard X positions for menu items (evenly spaced)
    static const uint8_t MENU_X_COL1 = 0;
    static const uint8_t MENU_X_COL2 = 26;
    static const uint8_t MENU_X_COL3 = 48;
    static const uint8_t MENU_X_COL4 = 64;
    static const uint8_t MENU_X_COL5 = 96;

    // Alternative spacing for 6 items
    static const uint8_t MENU_X_TIGHT_COL1 = 2;
    static const uint8_t MENU_X_TIGHT_COL2 = 25;
    static const uint8_t MENU_X_TIGHT_COL3 = 42;
    static const uint8_t MENU_X_TIGHT_COL4 = 60;
    static const uint8_t MENU_X_TIGHT_COL5 = 80;
    static const uint8_t MENU_X_TIGHT_COL6 = 105;

    // Common menu item widths
    static const uint8_t MENU_ITEM_WIDTH = 20;

    // Channel grid layout (for channel selectors)
    static const uint8_t CHANNEL_GRID_COLS = 3;
    static const uint8_t CHANNEL_GRID_ROWS = 2;
    static const uint8_t CHANNEL_AREA_WIDTH = 64;
    static const uint8_t CHANNEL_SPACING_X = 19;
    static const uint8_t CHANNEL_SPACING_Y = 19;
    static const uint8_t CHANNEL_OFFSET_X = 9;
    static const uint8_t CHANNEL_OFFSET_Y = 12;

    // Graphics area (right side)
    static const uint8_t GRAPHICS_AREA_X = 66;
    static const uint8_t GRAPHICS_AREA_WIDTH = 62;
    static const uint8_t GRAPHICS_AREA_HEIGHT = 42;
    static const uint8_t VERTICAL_SEPARATOR_X = 64;

    // Calculate channel position in 3x2 grid
    static void getChannelPosition(uint8_t channel, uint8_t &x, uint8_t &y) {
        uint8_t col = channel % CHANNEL_GRID_COLS;
        uint8_t row = channel / CHANNEL_GRID_COLS;
        x = col * CHANNEL_SPACING_X + CHANNEL_OFFSET_X;
        y = row * CHANNEL_SPACING_Y + CHANNEL_OFFSET_Y;
    }

    // Get center position for graphics area
    static void getGraphicsCenter(uint8_t &centerX, uint8_t &centerY) {
        centerX = GRAPHICS_AREA_X + GRAPHICS_AREA_WIDTH / 2;
        centerY = GRAPHICS_AREA_HEIGHT / 2;
    }
};

#endif
