#include "ExternalClock.hpp"

#if __linux__
#include <cmath>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#include <cmath>
#endif
#endif

#include <limits>

using namespace mpc::sequencer;

ExternalClock::ExternalClock()
{
    reset();
}

void ExternalClock::reset()
{
    previousIncomingPpqPosition = std::numeric_limits<double>::lowest();
    previousAbsolutePpqPosition = std::numeric_limits<double>::lowest();
    previousRelativePpqPosition = std::numeric_limits<double>::max();
    previousBpm = 0;
    previousPpqPositionOfLastBarStart = 0;
    ticksAreBeingProduced = false;
}

void ExternalClock::clearTicks()
{
    ticks.clear();
}

const FixedVector<uint16_t, 200>& ExternalClock::getTicksForCurrentBuffer()
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
    previousIncomingPpqPosition = ppqPosition;
    previousSampleRate = sampleRate;

    if (previousBpm == 0)
    {
        previousBpm = bpm;
    }

    // The transport has jumped back while playing, most likely because of looping
    if (ppqPositionOfLastBarStart < previousPpqPositionOfLastBarStart)
    {
        previousAbsolutePpqPosition = ppqPositionOfLastBarStart;
    }

    /*

       // This works as a hack for fixing the problem with VMPC2000XL's sequencer
       // not progressing after Reaper loops back to the starting position of a loop
       // that is smaller than the sequence.
    while (ppqPosition < previousAbsolutePpqPosition)
    {
        previousAbsolutePpqPosition -= 1;
    }
    */

    auto samplesInMinute = sampleRate * 60;
    auto samplesPerBeat = samplesInMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;

    if (bpm > previousBpm)
    {
        // When the tempo has increased drastically, there is the possibility of tick underflow.
        // Here we compute how many ticks should be created to compensate for this underflow.
        const double diffBetweenLastProcessedPpqAndCurrentPpq = ppqPosition - previousAbsolutePpqPosition;
        const double underflowTickCount = floor(diffBetweenLastProcessedPpqAndCurrentPpq * 96);

        for (int i = 0; i < underflowTickCount; i++)
        {
            // All underflowing ticks will be played back at sample index 0 within this buffer.
            ticks.push_back(0);
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

        auto relativePosition = fmod(ppqPositions[sample], subDiv);

        if (relativePosition < previousRelativePpqPosition)
        {
            ticks.push_back(sample);
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
    ticksAreBeingProduced = ticksAreBeingProduced || ticks.size() > 0;
}

bool ExternalClock::areTicksBeingProduced()
{
    return ticksAreBeingProduced;
}

const double ExternalClock::getLastKnownBpm()
{
    return previousBpm;
}

const uint32_t ExternalClock::getLastKnownSampleRate()
{
    return previousSampleRate;
}

const double ExternalClock::getLastKnownPpqPosition()
{
    return previousAbsolutePpqPosition;
}

const double ExternalClock::getLastProcessedIncomingPpqPosition()
{
    return previousIncomingPpqPosition;
}

