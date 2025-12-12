#pragma once

#include "sequencer/NoteRange.hpp"

#include "sequencer/TransportMessage.hpp"
#include "sequencer/SequenceMessage.hpp"
#include "sequencer/SongMessage.hpp"

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

    struct DeleteSequence
    {
        SequenceIndex sequenceIndex;
    };

    struct DeleteAllSequences
    {
    };

    struct CopySequence
    {
        SequenceIndex sourceIndex;
        SequenceIndex destIndex;
    };

    struct UndoSequence{};

    struct SetUndoSequenceAvailable
    {
        bool available;
    };

    struct CopyTrack
    {
        SequenceIndex sourceSequenceIndex;
        SequenceIndex destSequenceIndex;
        TrackIndex sourceTrackIndex;
        TrackIndex destTrackIndex;
    };

    using SequencerMessage =
        std::variant<TransportMessage, SequenceMessage, SongMessage, CopyEvents,
                     SwitchToNextSequence, SetSelectedSequenceIndex, CopyBars,
                     SetSelectedSongIndex, SetSelectedSongStepIndex,
                     DeleteSequence, CopySequence, UndoSequence, CopyTrack,
                     SetUndoSequenceAvailable, DeleteAllSequences>;
} // namespace mpc::sequencer
