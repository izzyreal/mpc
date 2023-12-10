#include "ExternalClock.hpp"

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
    const double timePerBeat = 60.0 / bpm; // Time for one beat in seconds
    const double timePerTick = timePerBeat / 96.0; // Time for one tick
    const double framesPerTick = timePerTick * sampleRate; // Frames per tick

    const double beatFraction = ppqPosAtStartOfBuffer - std::floor(ppqPosAtStartOfBuffer);
    const double beatFractionInFrames = beatFraction * timePerTick * sampleRate;
    const double startOfBeatWasThisManyFramesBeforeStartOfThisBuffer =
            -beatFractionInFrames;

    double tickFramePos = startOfBeatWasThisManyFramesBeforeStartOfThisBuffer;

    while (tickFramePos < 0)
    {
        tickFramePos += framesPerTick;
    }

    int tickCounter = 0;

    while (std::round(tickFramePos) < nFrames)
    {
        ticks[tickCounter++] = static_cast<int>(std::round<double>(tickFramePos));
        tickFramePos += framesPerTick;
    }
}

