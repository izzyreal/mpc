#include "ExternalClock.hpp"

using namespace mpc::sequencer;

void ExternalClock::reset()
{
    previousAbsolutePpqPosition = 0.0;
    previousRelativePpqPosition = 1.0;
}

void ExternalClock::clearTicks()
{
    for (double & tick : ticks)
    {
        tick = -1;
    }
}

std::vector<double>& ExternalClock::getTicksForCurrentBuffer()
{
    return ticks;
}

void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosition,
        int numSamples,
        int sampleRate,
        double bpm)
{
    auto samplesInMinute = sampleRate * 60;
    auto samplesPerBeat = samplesInMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;
    auto currentSubDiv = 1.0 / 96.0;

    double offset = 0.0;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        ppqPositions[sample] = ppqPosition + offset;
        offset += ppqPerSample;
    }

    int tickCounter = 0;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (ppqPositions[sample] < previousAbsolutePpqPosition)
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

    previousAbsolutePpqPosition = ppqPositions[numSamples - 1];
}
