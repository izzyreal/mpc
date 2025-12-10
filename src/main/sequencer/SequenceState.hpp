#pragma once

#include "sequencer/TrackState.hpp"
#include "sequencer/TimeSignature.hpp"

#include "MpcSpecs.hpp"
#include "IntTypes.hpp"

#include <array>

namespace mpc::sequencer
{
    struct SequenceState
    {
        std::string name;
        std::array<TrackState, Mpc2000XlSpecs::TOTAL_TRACK_COUNT> tracks;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
        BarIndex lastBarIndex;
        double initialTempo;
        bool loopEnabled;
        bool used;
        bool tempoChangeEnabled;
        BarIndex firstLoopBarIndex;
        BarIndex lastLoopBarIndex;

        SequenceState()
        {
            name.reserve(Mpc2000XlSpecs::MAX_SEQUENCE_NAME_LENGTH);
            initializeDefaults();
        }

        void initializeDefaults()
        {
            name.clear();
            tracks = {};
            timeSignatures = {};
            barLengths = {};
            lastBarIndex = NoBarIndex;
            initialTempo = 120.0;
            loopEnabled = true;
            used = false;
            tempoChangeEnabled = true;
            firstLoopBarIndex = BarIndex(0);
            lastLoopBarIndex = EndOfSequence;
        }
    };
} // namespace mpc::sequencer
