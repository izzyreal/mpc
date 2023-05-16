#include "MidiClockInput.hpp"

#include <cstdio>
#include <numeric>

using namespace mpc::audiomidi;

void MidiClockInput::handleTimingMessage()
{
    const auto now = std::chrono::high_resolution_clock::now();

    unsigned long delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now-previousNow).count();

    deltas[deltaPointer++] = delta;

    if (deltaPointer == DELTA_COUNT) deltaPointer = 0;

    previousNow = now;

    auto averageDelta = std::reduce(deltas.begin(), deltas.end(), 0.0) / deltas.size();

    auto bpm = 240000.0 / averageDelta;

    printf("BPM: %f\n", bpm);
}