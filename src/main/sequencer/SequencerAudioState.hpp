#pragma once

#include "IntTypes.hpp"
#include "SequencerAudioStateView.hpp"

namespace mpc::sequencer
{
    struct SequencerAudioState
    {
        // Absolute time in samples without wrapping since playback started
        TimeInSamples timeInSamples{0};
    };
} // namespace mpc::sequencer
