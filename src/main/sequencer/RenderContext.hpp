#pragma once

#include "PlaybackState.hpp"
#include "TransportStateView.hpp"

namespace mpc::sequencer
{
    class SequenceStateView;

    struct RenderContext
    {
        PlaybackState playbackState;
        TransportStateView transportStateView;
        std::shared_ptr<SequenceStateView> seq;
        TimeInSamples currentTime;
    };
}
