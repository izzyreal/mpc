#include <cassert>
#include "ExternalClock.hpp"

using namespace mpc::sequencer;

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
        double ppqPosAtStartOfBuffer,
        int nFrames,
        int sampleRate,
        double bpm)
{
    const double resolution = 96.0;

    const double samplesPerBeat = (60.0 * sampleRate) / bpm;
    const double samplesPerTick = samplesPerBeat / resolution;
    double tickPosAtStartOfBuffer = ppqPosAtStartOfBuffer * resolution;
    double ticksBeforeNextTick = -(tickPosAtStartOfBuffer - createdTicksSincePlayStart);
    double firstTickSample = ticksBeforeNextTick * samplesPerTick;

    int tickCounter = 0;

    for (double sample = firstTickSample; static_cast<int>(sample) < nFrames; sample += samplesPerTick)
    {
        ticks[tickCounter++] = static_cast<int>(sample); // Ensure sample is an integer
        createdTicksSincePlayStart++;
    }
}
