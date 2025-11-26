#pragma once

#include "TimeSignature.hpp"
#include "sequencer/PlaybackState.hpp"
#include "sequencer/EventState.hpp"

#include <array>
#include <vector>

namespace mpc::sequencer
{
    struct NonRtTrackState
    {
        std::vector<EventState> events;
    };

    struct NonRtSequenceState
    {
        std::array<NonRtTrackState, Mpc2000XlSpecs::TOTAL_TRACK_COUNT> tracks{};
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT>
            timeSignatures{};
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths{};
    };

    struct TransportState
    {
        PositionQuarterNotes positionQuarterNotes = NoPositionQuarterNotes;
        PositionQuarterNotes playStartPositionQuarterNotes = NoPositionQuarterNotes;
        bool sequencerRunning = false;
    };

    struct NonRtSequencerState
    {
        std::array<NonRtSequenceState, Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            sequences{};
        PlaybackState playbackState{};
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        TransportState transportState;
    };
} // namespace mpc::sequencer
