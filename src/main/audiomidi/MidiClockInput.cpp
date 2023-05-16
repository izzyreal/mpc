#include "MidiClockInput.hpp"

#include <cstdio>
#include <numeric>

using namespace mpc::audiomidi;

MidiClockInput::MidiClockInput()
{
    const double delta120Bpm = 1.0 / 48.0;
    std::fill(deltas.begin(), deltas.end(), delta120Bpm);
}

void MidiClockInput::handleTimingMessage(double bufOffsetSeconds)
{
    const auto now = std::chrono::high_resolution_clock::now();

    unsigned long delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now-previousNow).count();

    const double deltaInSeconds = (delta / 1000000000.0) + (bufOffsetSeconds / 1000);

    printf("D: %f\n", deltaInSeconds);

    deltas[deltaPointer++] = deltaInSeconds;

    if (deltaPointer == DELTA_COUNT) deltaPointer = 0;

    previousNow = now;

    auto averageDelta = std::reduce(deltas.begin(), deltas.end(), 0.0) / DELTA_COUNT;

//    printf("Avg delta: %f\n", averageDelta1);

    auto averageBpm = (1.0 / (averageDelta * 24.0)) * 60.0;

    printf("Avg BPM: %f\n", averageBpm);
}