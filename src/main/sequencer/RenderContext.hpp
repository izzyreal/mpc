#pragma once

#include "PlaybackState.hpp"

namespace mpc::sequencer
{
    class SequenceStateView;
    class Sequence;
    class Sequencer;

    struct RenderContext
    {
        PlaybackState playbackState;
        Sequencer *sequencer;
        std::shared_ptr<SequenceStateView> seq;
        TimeInSamples currentTime;
    };
}
