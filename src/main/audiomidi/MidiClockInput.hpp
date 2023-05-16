#pragma once

#include <vector>
#include <functional>

class MidiClockInput {
public:
    void handleTimingMessage(double framePos);
    void handleStartMessage();
    void handleStopMessage();

    std::function<void(const float newTempo)> onTempoChange;
    std::function<void()> onStart;
    std::function<void()> onStop;

    void setSampleRate(double sampleRate);

private:
    double sampleRate = 44100;
    unsigned int DELTA_COUNT = 20;
    double lastKnownTempo = 120.0;
    std::vector<double> deltas = std::vector<double>(DELTA_COUNT);
    unsigned int deltaPointer = 0;
    bool startIsArmed = false;
    double previousFrameCounter = 0;
};