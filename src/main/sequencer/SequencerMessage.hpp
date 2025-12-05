#pragma once

#include "sequencer/NoteRange.hpp"

#include "sequencer/TransportMessage.hpp"
#include "sequencer/SequenceMessage.hpp"

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

    struct SetSelectedSongIndex
    {
        SongIndex songIndex;
        SequenceIndex sequenceIndexOfFirstStep;
    };

    struct SetSelectedSongStepIndex
    {
        SongStepIndex songStepIndex;
        SequenceIndex sequenceIndexForThisStep;
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
    };

    struct CopyBars
    {
        SequenceIndex fromSeqIndex;
        SequenceIndex toSeqIndex;
        BarIndex copyFirstBar;
        BarIndex copyLastBar;
        int copyCount;
        BarIndex copyAfterBar;
        int destinationBarCount;
    };

    using SequencerMessage =
        std::variant<TransportMessage, SequenceMessage, CopyEvents,
                     SwitchToNextSequence, SetSelectedSequenceIndex, CopyBars,
                     SetSelectedSongIndex, SetSelectedSongStepIndex>;
} // namespace mpc::sequencer
