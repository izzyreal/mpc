#include "MidiClockInput.hpp"

#include <numeric>

void MidiClockInput::handleTimingMessage(double framePos)
{
    const bool firstCall = previousFrameCounter == 0;

    if (startIsArmed)
    {
        startIsArmed = false;
        onStart();
    }

    if (firstCall)
    {
        previousFrameCounter = framePos;
        return;
    }

    auto deltaInFrames = static_cast<double>(framePos - previousFrameCounter);

    deltas[deltaPointer++] = deltaInFrames;

    if (deltaPointer == DELTA_COUNT) deltaPointer = 0;

    previousFrameCounter = framePos;

    auto averageDeltaInFrames = std::reduce(deltas.begin(), deltas.end(), 0.0) / DELTA_COUNT;

    auto averageDeltaInSeconds = averageDeltaInFrames / sampleRate;

    double averageBpm = (1.0 / (averageDeltaInSeconds * 24.0)) * 60.0;

    if (lastKnownTempo != averageBpm)
    {
        onTempoChange(static_cast<float>(averageBpm));
        lastKnownTempo = averageBpm;
    }
}

void MidiClockInput::handleStartMessage()
{
    previousFrameCounter = 0;
    startIsArmed = true;
}

void MidiClockInput::handleStopMessage()
{
    onStop();
}

void MidiClockInput::setSampleRate(double sampleRateToUse)
{
    sampleRate = sampleRateToUse;
    const double deltaAtLastKnownTempoInFrames = (sampleRate / 48.0) * (120.0 / lastKnownTempo);
    std::fill(deltas.begin(), deltas.end(), deltaAtLastKnownTempoInFrames);
}
