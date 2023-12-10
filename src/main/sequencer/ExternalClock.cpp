#include "ExternalClock.hpp"
#include "Logger.hpp"

using namespace mpc::sequencer;

void ExternalClock::clearTicks()
{
    for (int & tick : ticks)
    {
        tick = -1;
    }
}

std::vector<int>& ExternalClock::getTicksForCurrentBuffer()
{
    return ticks;
}

void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosAtStartOfBuffer,
        int nFrames,
        int sampleRate,
        double bpm)
{
    auto samplesPerMinute = 60 * sampleRate;
    auto samplesPerBeat = samplesPerMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;
    double offset = 0.0;

    std::vector<double> ppqPositions(nFrames);

    for (int sample = 0; sample < nFrames; ++sample)
    {
        ppqPositions[sample] = ppqPosAtStartOfBuffer + offset;
        offset += ppqPerSample;
    }

    int tickCounter = 0;

    const double tolerance = 1e-3; // Define a small tolerance range
    double previousRelativePosition = 0.0; // Variable to track the previous relative position

    for (int sample = 0; sample < nFrames; ++sample)
    {
        auto relativePosition = fmod(ppqPositions[sample], 0.125);

        if (previousRelativePosition >= relativePosition &&
            relativePosition <= tolerance)
        {
            ticks[tickCounter++] = sample;
        }

        previousRelativePosition = relativePosition; // Update the previous position
    }
}

/*void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosAtStartOfBuffer,
        int nFrames,
        int sampleRate,
        double bpm)
{
    auto samplesPerBeat = (60.0 * sampleRate) / bpm;
    const double resolution = 4.0;
    auto samplesPerTick = samplesPerBeat / resolution;

    // Calculate the PPQ position of the first 1/96th tick in the buffer
    double firstTickPPQ = ceil(ppqPosAtStartOfBuffer * resolution) / resolution;
    double firstTickSample = (firstTickPPQ - ppqPosAtStartOfBuffer) * samplesPerBeat;

    int tickCounter = 0;

    // Find all ticks in the current buffer
    for (double sample = firstTickSample; sample < nFrames; sample += samplesPerTick)
    {
        ticks[tickCounter++] = static_cast<int>(sample);
    }
}*/
