#pragma once

#include "TimeSignature.hpp"
#include "sequencer/PlaybackState.hpp"
#include "sequencer/EventState.hpp"

#include <array>
#include <vector>

namespace mpc::sequencer
{
    struct TrackState
    {
        std::vector<EventState> events;
    };

    struct SequenceState
    {
        std::array<TrackState, Mpc2000XlSpecs::TOTAL_TRACK_COUNT> tracks{};
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT>
            timeSignatures{};
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths{};
        BarIndex lastBarIndex{NoBarIndex};
        double initialTempo = 120.0;
        bool loopEnabled = true;
        bool used = false;
        bool tempoChangeEnabled = true;
        BarIndex firstLoopBarIndex{0};
        BarIndex lastLoopBarIndex{EndOfSequence};
    };

    struct TransportState
    {
        PositionQuarterNotes positionQuarterNotes = NoPositionQuarterNotes;
        PositionQuarterNotes playStartPositionQuarterNotes =
            NoPositionQuarterNotes;
        bool sequencerRunning = false;
        bool recording = false;
        bool overdubbing = false;
        bool countEnabled = true;
    };

    struct SequencerState
    {
        std::array<SequenceState, Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            sequences{};
        PlaybackState playbackState{};
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        TransportState transport;
    };
} // namespace mpc::sequencer
