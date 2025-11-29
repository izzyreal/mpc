#pragma once

#include "PlaybackState.hpp"

namespace mpc::sequencer
{
    class Sequence;
    class Sequencer;

    struct RenderContext
    {
        PlaybackState playbackState;
        Sequencer *sequencer;
        Sequence *seq;
        TimeInSamples currentTime;
    };
}
