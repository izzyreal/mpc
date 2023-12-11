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

/*void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosAtStartOfBuffer,
        int nFrames,
        int sampleRate,
        double bpm)
{
    auto samplesPerMinute = 60 * sampleRate;
    auto samplesPerBeat = samplesPerMinute / bpm;
    auto ppqPerSample = 1.0 / samplesPerBeat;
    double offset = 0.0;

    std::vector<double> ppqPositions(nFrames);

    for (int sample = 0; sample < nFrames; ++sample)
    {
        ppqPositions[sample] = ppqPosAtStartOfBuffer + offset;
        offset += ppqPerSample;
    }

    int tickCounter = 0;

    const double tolerance = 1e-3; // Define a small tolerance range
    double previousRelativePosition = 0.0; // Variable to track the previous relative position

    for (int sample = 0; sample < nFrames; ++sample)
    {
        auto relativePosition = fmod(ppqPositions[sample], 0.25);

        if (previousRelativePosition >= relativePosition &&
            relativePosition <= tolerance)
        {
            ticks[tickCounter++] = sample;
        }

        previousRelativePosition = relativePosition; // Update the previous position
    }
}*/

void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosAtStartOfBuffer,
        int nFrames,
        int sampleRate,
        double bpm)
{
    auto samplesPerBeat = (60.0 * sampleRate) / bpm;
    const double resolution = 4.0;
    const auto samplesPer16th = samplesPerBeat / resolution;
    const auto samplesPerTick = samplesPerBeat / 96.0;

    double firstTickPPQ = ceil(ppqPosAtStartOfBuffer * resolution) / resolution;
    double firstTickSample = (firstTickPPQ - ppqPosAtStartOfBuffer) * samplesPerBeat;

    int tickCounter = 0;

    for (double sample = firstTickSample; sample < nFrames; sample += samplesPer16th)
    {
        for (int subDivision = 0; subDivision < 24; subDivision++)
        {
            ticks[tickCounter++] = sample + (subDivision * samplesPerTick);
        }
    }
}
