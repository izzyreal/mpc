#include "ExternalClock.hpp"

using namespace mpc::sequencer;

void ExternalClock::reset()
{
    previousAbsolutePpqPosition = -1.0;
    previousRelativePpqPosition = 1.0;
}

void ExternalClock::clearTicks()
{
    for (int32_t& tick : ticks)
    {
        tick = -1;
    }
}

std::vector<int32_t>& ExternalClock::getTicksForCurrentBuffer()
{
    return ticks;
}

void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosition,
        int nFrames,
        int sampleRate,
        double bpm)
{
    auto samplesInMinute = sampleRate * 60;
    auto samplesPerBeat = samplesInMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;
    auto currentSubDiv = 1.0 / 96.0;

    double offset = 0.0;

    for (int sample = 0; sample < nFrames; ++sample)
    {
        ppqPositions[sample] = ppqPosition + offset;
        offset += ppqPerSample;
    }

    int tickCounter = 0;

    for (int sample = 0; sample < nFrames; ++sample)
    {
        if (ppqPositions[sample] <= previousAbsolutePpqPosition)
        {
            continue;
        }

        auto relativePosition = fmod(ppqPositions[sample], currentSubDiv);

        if (relativePosition < previousRelativePpqPosition)
        {
            ticks[tickCounter++] = sample;
        }

        previousRelativePpqPosition = relativePosition;
    }

    previousAbsolutePpqPosition = ppqPositions[nFrames - 1];
}
