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

//void ExternalClock::computeTicksForCurrentBuffer(
//        double ppqPosAtStartOfBuffer,
//        int nFrames,
//        int sampleRate,
//        double bpm)
//{
//    auto samplesPerMinute = 60 * sampleRate;
//    auto samplesPerBeat = samplesPerMinute / bpm;
//    auto samplesPerTick = samplesPerBeat / 96.0;
//    auto ppqPerSample = 1.0 / samplesPerBeat;
//    double offset = 0.0;
//
//    std::vector<double> ppqPositions(nFrames);
//
//    for (int sample = 0; sample < nFrames; ++sample)
//    {
//        ppqPositions[sample] = ppqPosAtStartOfBuffer + offset;
//        offset += ppqPerSample;
//    }
//
//    int tickCounter = 0;
//
//    const double tolerance = 1e-3; // Define a small tolerance range
//    double previousRelativePosition = 0.0; // Variable to track the previous relative position
//
//    for (int sample = 0; sample < nFrames; ++sample)
//    {
//        auto relativePosition = fmod(ppqPositions[sample], 0.25);
//
//        if (previousRelativePosition >= relativePosition &&
//            relativePosition <= tolerance)
//        {
//            for (int i = 0; i < 24; i++)
//            {
//                ticks[tickCounter++] = sample + (samplesPerTick * i);
//            }
//        }
//
//        previousRelativePosition = relativePosition; // Update the previous position
//    }
//}

void ExternalClock::computeTicksForCurrentBuffer(
        double ppqPosAtStartOfBuffer,
        int nFrames,
        int sampleRate,
        double bpm)
{
    const double samplesPerBeat = (60.0 * sampleRate) / bpm;
    const double resolution = 96.0;
    const double samplesPerTick = samplesPerBeat / resolution;

    // Calculate the PPQ position in ticks
    const double ppqPosTicks = ppqPosAtStartOfBuffer * resolution;

    // Calculate the sample position of the first tick in this buffer
    double firstTickPpq = ceil(ppqPosTicks) - ppqPosTicks;
    double firstTickSample = firstTickPpq * samplesPerTick;

    // Correcting firstTickSample based on the lastSamplePosition
    if (lastSamplePosition > 0) {
        firstTickSample = fmod(lastSamplePosition, samplesPerTick);
        if (firstTickSample != 0) {
            firstTickSample = samplesPerTick - firstTickSample;
        }
    }

    int tickCounter = 0;

    // Adjust the loop to iterate over the buffer
    for (double sample = firstTickSample; sample < nFrames; sample += samplesPerTick)
    {
        ticks[tickCounter++] = static_cast<int>(sample); // Ensure sample is an integer
    }

    // Update the lastSamplePosition for the next buffer
    lastSamplePosition += nFrames;
}
