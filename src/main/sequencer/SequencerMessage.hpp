#pragma once

#include "sequencer/NoteRange.hpp"

#include "sequencer/TransportMessage.hpp"
#include "sequencer/SequenceMessage.hpp"

#include <functional>
#include <variant>

namespace mpc::sequencer
{
    struct SwitchToNextSequence
    {
        SequenceIndex sequenceIndex;
    };

    struct SetSelectedSequenceIndex
    {
        SequenceIndex sequenceIndex;
        bool setPositionTo0 = true;
    };

    struct CopyEvents
    {
        Tick sourceStartTick;
        Tick sourceEndTick;
        SequenceIndex sourceSequenceIndex;
        SequenceIndex destSequenceIndex;
        TrackIndex sourceTrackIndex;
        TrackIndex destTrackIndex;
        Tick destStartTick;
        bool copyModeMerge;
        int copyCount;
        NoteRange sourceNoteRange{};
        std::function<EventId()> generateEventId;
    };

    using SequencerMessage = std::variant<TransportMessage, SequenceMessage,

                                          CopyEvents, SwitchToNextSequence,
                                          SetSelectedSequenceIndex>;
} // namespace mpc::sequencer
