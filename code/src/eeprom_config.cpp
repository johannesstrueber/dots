#include <Arduino.h>
#include <EEPROM.h>

// EEPROM Address Map
// 0-383: Matrix patterns (16 patterns * 24 bytes each)
// 384-396: Settings (13 bytes)
// 397-421: Euclidean settings (25 bytes)
// 422-427: Channel dividers (6 bytes)
// 397-421: Euclidean settings (25 bytes)

struct Settings {
    uint8_t bpm;              // BPM value
    bool clkMode;             // Clock mode (internal/external)
    bool outMode;             // Output mode (trigger/gate)
    uint8_t bootMode;         // Boot mode selection
    uint8_t resetMode;        // Reset mode
    int8_t msDelay;           // Delay in ms
    uint8_t ranMode;          // Random trigger mode
    uint8_t eucPatternLength; // Euclidean pattern length
    uint8_t reserved[4];      // Reserved for future use
};

struct EuclideanTrack {
    uint8_t steps;
    uint8_t hits;
    uint8_t rotation;
    bool mute;
    uint32_t pattern;
};

struct EuclideanSettings {
    EuclideanTrack tracks[6];
    uint8_t currentTrack;
};

// ========================================
// CUSTOMIZE YOUR DEFAULT SETTINGS HERE
// ========================================

Settings defaultSettings = {
    .bpm = 120,              // Default BPM
    .clkMode = false,        // true = internal, false = external
    .outMode = true,         // true = trigger, false = gate
    .bootMode = 4,           // 0=main menu, 1=step seq, 2=euclidean, 3=random, 4=clock div
    .resetMode = 1,          // Reset mode
    .msDelay = 0,            // No delay
    .ranMode = 0,            // All channels mode
    .eucPatternLength = 8,   // 8-step Euclidean patterns
    .reserved = {0, 0, 0, 0} // Reserved bytes
};

// ========================================
// CUSTOMIZE YOUR EUCLIDEAN SEQ PATTERN HERE
// ========================================

EuclideanSettings defaultEuclidean = {.tracks =
                                          {
                                              {16, 4, 0, false, 0}, // Track 1: 4/4 kick (4 hits in 16 steps)
                                              {16, 2, 4, false, 0}, // Track 2: Backbeat snare (2 hits offset by 4)
                                              {16, 8, 0, false, 0}, // Track 3: Eighth-note hi-hat (8 hits in 16)
                                              {8, 3, 0, false, 0},  // Track 4: Triplet pattern (3 hits in 8)
                                              {12, 5, 2, false, 0}, // Track 5: Cross-rhythm (5 in 12, offset 2)
                                              {16, 7, 1, false, 0}  // Track 6: Syncopated (7 in 16, offset 1)
                                          },
                                      .currentTrack = 0};

void writeDefaultStepPatterns() {
    const uint8_t rows = 6;
    const uint8_t cols = 16;
    const uint8_t pages = 8;
    const uint8_t seqMatrixSize = (rows * cols * pages) / 8;

    byte tempMatrix[seqMatrixSize];

    for (int i = 0; i < seqMatrixSize; i++) {
        tempMatrix[i] = 0xFF;
    }

    auto setBit = [&](int page, int row, int col) {
        int index = page * rows * cols + row * cols + col;
        int byteIndex = index / 8;
        int bitIndex = index % 8;
        tempMatrix[byteIndex] &= ~(1 << bitIndex); // Clear bit for ON
    };

    // ========================================
    // CUSTOMIZE YOUR STEP SEQ PATTERN HERE
    // ========================================

    // Each page = 16 steps, so 128 steps = 8 pages
    // 1 = ON, 0 = OFF for easy visualization

    int page0[6][16] = {// CH1 :     1---2---3---4---
                        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                        // CH2 :    ----1-------2---
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                        // CH3 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH4 :     ------1---------
                        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH5 :    1---------------
                        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :     --1---1---1---1-
                        {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}};

    int page1[6][16] = {// CH1 :     1---2---3---4---
                        {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                        // CH2 :    ----1-------2---
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                        // CH3 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH4 :     --------1-------
                        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                        // CH5 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :     --1---1---1---1-
                        {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}};

    int page2[6][16] = {// CH1 :     1---2---3---4---
                        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                        // CH2 :    ----1-------2-3-
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
                        // CH3 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH4 :     ------1---------
                        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH5 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :     --1---1---1-1-1-
                        {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0}};

    int page3[6][16] = {// CH1 :     1---2---3---4---
                        {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                        // CH2 :    ----1-------2---
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                        // CH3 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH4 :     --------1-------
                        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                        // CH5 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :     --1---1---1---1-
                        {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}};

    int page4[6][16] = {// CH1 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH2 :     1---2---3---4---
                        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                        // CH3 :    ----1-------2---
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                        // CH4 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH5 :     ------1---------
                        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :    1---------------
                        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    int page5[6][16] = {// CH1 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH2 :     1---2---3---4---
                        {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                        // CH3 :    ----1-------2---
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                        // CH4 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH5 :     --------1-------
                        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    int page6[6][16] = {// CH1 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH2 :     1---2---3---4---
                        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                        // CH3 :    ----1-------2-3-
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
                        // CH4 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH5 :     ------1---------
                        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    int page7[6][16] = {// CH1 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        // CH2 :     1---2---3---4---
                        {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                        // CH3 :    ----1-------2---
                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                        // CH4 :   1-1-1-1-1-1-1-1-
                        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        // CH5 :     --------1-------
                        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                        // CH6 :    ----------------
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 16; col++) {
            if (page0[row][col])
                setBit(0, row, col);
            if (page1[row][col])
                setBit(1, row, col);
            if (page2[row][col])
                setBit(2, row, col);
            if (page3[row][col])
                setBit(3, row, col);
            if (page4[row][col])
                setBit(4, row, col);
            if (page5[row][col])
                setBit(5, row, col);
            if (page6[row][col])
                setBit(6, row, col);
            if (page7[row][col])
                setBit(7, row, col);
        }
    }

    for (int i = 0; i < seqMatrixSize; i++) {
        EEPROM.write(i, tempMatrix[i]);
    }
}

void clearEEPROM() {
    for (int i = 0; i < 96; i++) {
        EEPROM.write(i, 0);
    }
    for (int i = 96; i < 384; i++) {
        EEPROM.write(i, 0);
    }

    for (int i = 384; i <= 396; i++) {
        EEPROM.write(i, 0);
    }

    for (int i = 397; i <= 421; i++) {
        EEPROM.write(i, 0);
    }
}
void writeSettings() {
    clearEEPROM();

    EEPROM.write(384, 0);                         // seqCurrentPage
    EEPROM.write(385, 16);                        // seqCurrentLength
    EEPROM.write(386, 0);                         // seqCurrentOffset
    EEPROM.write(387, defaultSettings.resetMode); // resetMode
    EEPROM.write(388, false);                     // isPause
    EEPROM.write(389, defaultSettings.bpm);       // bpm
    EEPROM.write(390, defaultSettings.clkMode);   // clkMode
    EEPROM.write(391, 120);                       // intClock
    EEPROM.write(392, defaultSettings.bootMode);  // bootMode
    EEPROM.write(393, defaultSettings.outMode);   // outMode
    EEPROM.write(395, defaultSettings.ranMode);   // ranMode
    EEPROM.write(396, 6);                         // ranActiveChannels
    EEPROM.write(397, 16);                        // eucPatternLength (default 16 steps)

    int addr = 398;
    for (int i = 0; i < 6; i++) {
        EEPROM.write(addr++, defaultEuclidean.tracks[i].steps);
        EEPROM.write(addr++, defaultEuclidean.tracks[i].hits);
        EEPROM.write(addr++, defaultEuclidean.tracks[i].rotation);
        EEPROM.write(addr++, defaultEuclidean.tracks[i].mute ? 1 : 0);
    }

    uint8_t defaultChannelDividers[6] = {1, 2, 4, 3, 6, 8};
    for (int i = 0; i < 6; i++) {
        EEPROM.write(422 + i, defaultChannelDividers[i]);
    }

    writeDefaultStepPatterns();
}

void setup() {
    writeSettings();
}

void loop() {
}
