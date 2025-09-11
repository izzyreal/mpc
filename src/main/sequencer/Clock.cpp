#include "Clock.hpp"

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

Clock::Clock()
{
    reset();
}

void Clock::reset()
{
    previousHostPositionAtStartOfBufferQuarterNotes = std::numeric_limits<double>::lowest();
    previousAbsolutePositionQuarterNotes = std::numeric_limits<double>::lowest();
    previousRelativePositionQuarterNotes = std::numeric_limits<double>::max();
    previousTimeInSamples = std::numeric_limits<int64_t>::lowest();
    previousBufferSize = 0;
    previousBpm = 0;
    ticksAreBeingProduced = false;
    jumpOccurredInLastBuffer = false;
}

void Clock::clearTicks()
{
    ticks.clear();
}

const FixedVector<uint16_t, 200>& Clock::getTicksForCurrentBuffer()
{
    return ticks;
}

void Clock::computeTicksForCurrentBuffer(
        const double hostPositionAtStartOfBufferQuarterNotes,
        const int nFrames,
        const int sampleRate,
        const double bpm,
        const int64_t timeInSamples)
{
    resetJumpOccurredInLastBuffer();
    clearTicks();

    bool jumpOccurred = false;

    if (timeInSamples != std::numeric_limits<int64_t>::lowest())
    {
        if (previousTimeInSamples != std::numeric_limits<int64_t>::lowest() &&
            previousBufferSize != 0)
        {
            if (timeInSamples != previousTimeInSamples + previousBufferSize &&
                timeInSamples != previousTimeInSamples)
            {
                // We have jumped backwards or forwards and need to correct the sequencer's
                // position and the event indices in the tracks.
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

    previousHostPositionAtStartOfBufferQuarterNotes = hostPositionAtStartOfBufferQuarterNotes;
    previousSampleRate = sampleRate;

    positionsInQuarterNotes.clear();

    if (previousBpm == 0)
    {
        previousBpm = bpm;
    }

    const auto samplesInMinute = sampleRate * 60;
    const auto samplesPerBeat = samplesInMinute / bpm;
    const auto quarterNotesPerSample = 1.0 / samplesPerBeat;

    if (bpm > previousBpm)
    {
        // When the tempo has increased drastically, there is the possibility of tick underflow.
        // Here we compute how many ticks should be created to compensate for this underflow.
        const double diffBetweenLastProcessedAndCurrentPos = hostPositionAtStartOfBufferQuarterNotes - previousAbsolutePositionQuarterNotes;
        const double underflowTickCount = floor(diffBetweenLastProcessedAndCurrentPos * Sequencer::TICKS_PER_QUARTER_NOTE);

        for (int i = 0; i < underflowTickCount; i++)
        {
            // All underflowing ticks will be played back at sample index 0 within this buffer.
            ticks.push_back(0);
        }
    }

    double offset = 0.0;

    for (int sample = 0; sample < nFrames; ++sample)
    {
        positionsInQuarterNotes.push_back(hostPositionAtStartOfBufferQuarterNotes + offset);
        offset += quarterNotesPerSample;
    }

    for (int sample = 0; sample < nFrames; ++sample)
    {
        // When the tempo has decreased drastically, some hosts report a position in quarter notes that is
        // lower than what was already processed in the previous buffer. Here we make sure
        // we do not process the already processed positions.
        if (positionsInQuarterNotes[sample] <= previousAbsolutePositionQuarterNotes)
        {
            continue;
        }

        auto relativePosition = fmod(positionsInQuarterNotes[sample], subDiv);

        if (relativePosition < 0)
        {
            relativePosition += subDiv;
        }

        if (relativePosition < previousRelativePositionQuarterNotes)
        {
            ticks.push_back(sample);
        }

        previousRelativePositionQuarterNotes = relativePosition;
    }

    if (positionsInQuarterNotes[nFrames - 1] > previousAbsolutePositionQuarterNotes)
    {
        // This should happen any time the tempo has not drastically decreased.
        previousAbsolutePositionQuarterNotes = positionsInQuarterNotes[nFrames - 1];
    }

    previousBpm = bpm;
    ticksAreBeingProduced = ticksAreBeingProduced || ticks.size() > 0;
}

bool Clock::areTicksBeingProduced()
{
    return ticksAreBeingProduced;
}

const double Clock::getLastProcessedHostPositionQuarterNotes()
{
    return previousHostPositionAtStartOfBufferQuarterNotes;
}

bool Clock::didJumpOccurInLastBuffer()
{
    return jumpOccurredInLastBuffer;
}

void Clock::resetJumpOccurredInLastBuffer()
{
    jumpOccurredInLastBuffer = false;
}

void Clock::generateTransportInfo(const float tempo,
                                  const uint32_t sampleRate,
                                  const uint16_t numSamples,
                                  const double playStartPositionQuarterNotes)
{
        const double lastProcessedPositionQuarterNotes = getLastProcessedHostPositionQuarterNotes();
        const auto beatsPerFrame = 1.0 / ((1.0/(tempo/60.0)) * sampleRate);

        // This approach does not 100% mimic the values that Reaper produces. Although it comes close, Reaper's values are 100% the same if we would
        // compute without accumulating quarter notes, and instead keep track of the number of buffers that already passed.
        // I'm currently not sure if this actually needs to be addressed. My gut is that both implementations are more than accurate and correct
        // enough for most artistic intents and purposes.
        const auto newPositionQuarterNotes =
            lastProcessedPositionQuarterNotes == std::numeric_limits<double>::lowest() ?
            playStartPositionQuarterNotes :
            (lastProcessedPositionQuarterNotes + (numSamples * beatsPerFrame));

        computeTicksForCurrentBuffer(
                    newPositionQuarterNotes,
                    numSamples,
                    sampleRate,
                    tempo,
                    std::numeric_limits<int64_t>::lowest());
}

void Clock::processBufferInternal(const float tempo,
                                  const uint32_t sampleRate,
                                  const uint16_t numSamples,
                                  const double playStartPositionQuarterNotes)
{
        const double lastProcessedPositionQuarterNotes = getLastProcessedHostPositionQuarterNotes();
        const auto beatsPerFrame = 1.0 / ((1.0/(tempo/60.0)) * sampleRate);

        // This approach does not 100% mimic the values that Reaper produces. Although it comes close, Reaper's values are 100% the same if we would
        // compute without accumulating quarter notes, and instead keep track of the number of buffers that already passed.
        // I'm currently not sure if this actually needs to be addressed. My gut is that both implementations are more than accurate and correct
        // enough for most artistic intents and purposes.
        const auto newPositionQuarterNotes =
            lastProcessedPositionQuarterNotes == std::numeric_limits<double>::lowest() ?
            playStartPositionQuarterNotes :
            (lastProcessedPositionQuarterNotes + (numSamples * beatsPerFrame));

        computeTicksForCurrentBuffer(
                    newPositionQuarterNotes,
                    numSamples,
                    sampleRate,
                    tempo,
                    std::numeric_limits<int64_t>::lowest());
}

void Clock::processBufferExternal(
        const double hostPositionAtStartOfBufferQuarterNotes,
        const int nFrames,
        const int sampleRate,
        const double bpm,
        const int64_t timeInSamples)
{
    computeTicksForCurrentBuffer(hostPositionAtStartOfBufferQuarterNotes,
                                 nFrames,
                                 sampleRate,
                                 bpm,
                                 timeInSamples);
}

