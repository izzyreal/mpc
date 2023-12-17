#include "ExternalClock.hpp"

#if __linux__
#include <cmath>
#endif

using namespace mpc::sequencer;

void ExternalClock::reset()
{
    previousAbsolutePpqPosition = -1.0;
    previousRelativePpqPosition = 1.0;
    previousBpm = 0;
    previousPpqPositionOfLastBarStart = 0;
    ticksAreBeingProduced = false;
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
        double ppqPositionOfLastBarStart,
        int nFrames,
        int sampleRate,
        double bpm)
{
    if (previousBpm == 0)
    {
        previousBpm = bpm;
    }

    // The transport has jumped back while playing, most likely because of looping
    if (ppqPositionOfLastBarStart < previousPpqPositionOfLastBarStart)
    {
        previousAbsolutePpqPosition = ppqPositionOfLastBarStart;
    }

    auto samplesInMinute = sampleRate * 60;
    auto samplesPerBeat = samplesInMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;
    auto currentSubDiv = 1.0 / 96.0;

    int tickCounter = 0;

    if (bpm > previousBpm)
    {
        // When the tempo has increased drastically, there is the possibility of tick underflow.
        // Here we compute how many ticks should be created to compensate for this underflow.
        const double diffBetweenLastProcessedPpqAndCurrentPpq = ppqPosition - previousAbsolutePpqPosition;
        const double underflowTickCount = floor(diffBetweenLastProcessedPpqAndCurrentPpq * 96);

        for (int i = 0; i < underflowTickCount; i++)
        {
            // All underflowing ticks will be played back at sample index 0 within this buffer.
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
        // When the tempo has decreased drastically, some hosts report a ppqPosition that is
        // lower than what was already processed in the previous buffer. Here we make sure
        // we do not process the already processed positions.
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
        // This should happen any time the tempo has not drastically decreased.
        previousAbsolutePpqPosition = ppqPositions[nFrames - 1];
    }

    previousBpm = bpm;
    previousPpqPositionOfLastBarStart = ppqPositionOfLastBarStart;
    ticksAreBeingProduced = ticksAreBeingProduced || tickCounter > 0;
}

bool ExternalClock::areTicksBeingProduced()
{
    return ticksAreBeingProduced;
}