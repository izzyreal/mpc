#include "ExternalClock.hpp"

#include <Logger.hpp>

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
    auto samplesPerBeat = (60.0 * sampleRate) / bpm;
    double firstBeatPPQ = ceil(ppqPosAtStartOfBuffer);
    double firstBeatSample = (firstBeatPPQ - ppqPosAtStartOfBuffer) * samplesPerBeat;

    int tickCounter = 0;

    const auto samplesPerTick = samplesPerBeat / 96.0;

    double sample = firstBeatSample;

    while (sample < nFrames)
    {
        for (int subDivision = 0; subDivision < 96; subDivision++)
        {
            ticks[tickCounter++] = sample + (subDivision * samplesPerTick);
//            if (subDivision > 0) {
//                auto candidate = ticks[subDivision] - ticks[subDivision - 1];
//                if (candidate < 229 || candidate > 230) {
//                    MLOG("messed up external clock: " + std::to_string(candidate));
//                }
//            }
        }

        sample += samplesPerBeat;
    }
}
