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
#include <string>

#include <Logger.hpp>

#include <cassert>

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
    previousTimeInSamples = std::numeric_limits<int64_t>::lowest();
    previousBufferSize = 0;
    previousBpm = 0;
    previousPpqPositionOfLastBarStart = 0;
    ticksAreBeingProduced = false;
    jumpOccurredInLastBuffer = false;
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
        const double ppqPosition,
        const double ppqPositionOfLastBarStart,
        const int nFrames,
        const int sampleRate,
        const double bpm,
        const int64_t timeInSamples)
{
    bool jumpOccurred = false;

    if (timeInSamples != std::numeric_limits<int64_t>::lowest())
    {
        if (previousTimeInSamples != std::numeric_limits<int64_t>::lowest() &&
            previousBufferSize != 0)
        {
            if (timeInSamples != previousTimeInSamples + previousBufferSize &&
                timeInSamples != previousTimeInSamples)
            {
                /* We have jumped backwards or forwards and need to correct the sequencer PPQ pos and the event indices in the tracks */
                jumpOccurred = true;
            }
        }
        previousTimeInSamples = timeInSamples;
        previousBufferSize = nFrames;
    }

    if (jumpOccurred)
    {
        reset();
    }

    jumpOccurredInLastBuffer = jumpOccurred;

    previousIncomingPpqPosition = ppqPosition;
    previousSampleRate = sampleRate;

    ppqPositions.clear();

    if (previousBpm == 0)
    {
        previousBpm = bpm;
    }

    auto samplesInMinute = sampleRate * 60;
    auto samplesPerBeat = samplesInMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;

    if (bpm > previousBpm)
    {
        assert(!jumpOccurred);
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
        ppqPositions.push_back(ppqPosition + offset);
        offset += ppqPerSample;
    }

    for (int sample = 0; sample < nFrames; ++sample)
    {
        // When the tempo has decreased drastically, some hosts report a ppqPosition that is
        // lower than what was already processed in the previous buffer. Here we make sure
        // we do not process the already processed positions.
        if (ppqPositions[sample] <= previousAbsolutePpqPosition)
        {
            assert(!jumpOccurred);
            continue;
        }

        auto relativePosition = fmod(ppqPositions[sample], subDiv);

        if (relativePosition < 0)
        {
            relativePosition += subDiv;
        }

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

const double ExternalClock::getLastProcessedIncomingPpqPosition()
{
    return previousIncomingPpqPosition;
}

bool ExternalClock::didJumpOccurInLastBuffer()
{
    return jumpOccurredInLastBuffer;
}

void ExternalClock::resetJumpOccurredInLastBuffer()
{
    jumpOccurredInLastBuffer = false;
}

