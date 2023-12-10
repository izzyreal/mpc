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

    // Current position in frames
    double currentFrame = (ppqPosAtStartOfBuffer - std::floor(ppqPosAtStartOfBuffer)) * timePerBeat * sampleRate;

    int tickCounter = 0;

    while(currentFrame < nFrames) {
        ticks[tickCounter++] = static_cast<int>(std::round(currentFrame));
        currentFrame += framesPerTick; // Move to the next tick
    }
}
