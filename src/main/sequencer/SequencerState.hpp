#pragma once

#include "IntTypes.hpp"
#include "SequencerStateView.hpp"

namespace mpc::sequencer
{
    struct SequencerState
    {
        // Absolute time in samples without wrapping since playback started
        TimeInSamples timeInSamples{0};
    };
} // namespace mpc::sequencer
