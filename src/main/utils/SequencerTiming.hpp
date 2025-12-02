#pragma once

#include "IntTypes.hpp"

#include <vector>

namespace mpc::sequencer
{
    class Sequence;
    class SequenceStateView;
} // namespace mpc::sequencer

namespace mpc::utils
{
    struct TempoChangeEventData
    {
        double tick;
        double tempo;
    };

    struct SequenceTimingData
    {
        double initialTempo = 120.0;

        // Sorted by tick
        std::vector<TempoChangeEventData> tempoChanges;

        bool loopEnabled = false;
        double loopStartTick = 0.0;
        double loopEndTick = 0.0;

        double lastTick = 0.0; // total sequence length in ticks
    };

    SequenceTimingData
    getSequenceTimingData(const sequencer::SequenceStateView &);

    double getTickCountForFrames(const SequenceTimingData &s, double firstTick,
                                 int frameCount, int sr);

    int getFrameCountForTicks(const SequenceTimingData &s, double firstTick,
                              double tickCount, int sr);

    int getEventTimeInSamples(const sequencer::SequenceStateView &,
                              int eventTick, int currentTimeSamples,
                              SampleRate);

    int getEventTimeInSamples(const SequenceTimingData &snapshotTimingData,
                              int blockStartTick, int eventTick,
                              int64_t strictValidFromSamples,
                              SampleRate sampleRate);

} // namespace mpc::utils