#pragma once

#include "IntTypes.hpp"
#include "SequencerStateView.hpp"

namespace mpc::sequencer
{
    struct SequencerState
    {
        // Absolute time in samples without wrapping since playback started
        TimeInSamples timeInSamples{NoTimeInSamples};
    };
} // namespace mpc::sequencer
