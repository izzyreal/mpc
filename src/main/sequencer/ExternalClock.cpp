#include "ExternalClock.hpp"

using namespace mpc::sequencer;

void ExternalClock::reset()
{
    previousAbsolutePpqPosition = -1.0;
    previousRelativePpqPosition = 1.0;
    previousBpm = 0;
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
    if (previousBpm == 0)
    {
        previousBpm = bpm;
    }

    auto samplesInMinute = sampleRate * 60;
    auto samplesPerBeat = samplesInMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;
    auto currentSubDiv = 1.0 / 96.0;

    int tickCounter = 0;

    if (bpm > previousBpm)
    {
        const double diffBetweenLastProcessedPpqAndCurrentPpq = ppqPosition - ppqPositions[nFrames - 1];
        const double underflowTickCount = floor(diffBetweenLastProcessedPpqAndCurrentPpq * 96);

        for (int i = 0; i < underflowTickCount; i++)
        {
            ticks[tickCounter++] = 0;
        }
    }

    double offset = 0.0;

    for (int sample = 0; sample < nFrames; ++sample)
    {
        ppqPositions[sample] = ppqPosition + offset;
        offset += ppqPerSample;
    }

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

    if (ppqPositions[nFrames - 1] > previousAbsolutePpqPosition)
    {
        previousAbsolutePpqPosition = ppqPositions[nFrames - 1];
    }

    previousBpm = bpm;
}
