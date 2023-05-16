#include "MidiClockInput.hpp"

#include <cstdio>
#include <numeric>

using namespace mpc::audiomidi;

void MidiClockInput::handleTimingMessage(double bufOffsetMs)
{
    const auto now = std::chrono::high_resolution_clock::now();

    const bool firstCall = previousNow == zero;

    if (firstCall)
    {
        previousNow = now;
        return;
    }

    unsigned long delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now-previousNow).count();

    const double deltaInSeconds = (delta / 1000000000.0) + (bufOffsetMs * 1000);

    deltas[deltaPointer++] = deltaInSeconds;

    if (deltaPointer == DELTA_COUNT) deltaPointer = 0;

    previousNow = now;

    auto averageDelta = std::reduce(deltas.begin(), deltas.end(), 0.0) / DELTA_COUNT;

//    printf("Avg delta: %f\n", averageDelta1);

    auto averageBpm = (1.0 / (averageDelta * 24.0)) * 60.0;

    printf("Avg BPM: %f\n", averageBpm);
}