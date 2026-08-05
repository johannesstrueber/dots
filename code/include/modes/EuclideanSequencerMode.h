#ifndef EUCLIDEAN_SEQUENCER_MODE_H
#define EUCLIDEAN_SEQUENCER_MODE_H

#include <Arduino.h>

struct EuclideanTrack {
    uint8_t steps;
    uint8_t hits;
    uint8_t rotation;
    bool mute;
    uint32_t pattern;
};

extern EuclideanTrack eucTracks[6];
extern uint8_t eucPatternLength;

void updateEuclideanPatterns();
void renderEuclideanSequencerScreen();
void updateEuclideanSequencer();

void saveEuclideanSequencerSettings();
void loadEuclideanSequencerSettings();

#endif
