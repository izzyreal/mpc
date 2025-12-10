#pragma once

#include "sequencer/SongStateView.hpp"
#include "sequencer/TransportStateView.hpp"
#include "sequencer/SequencerState.hpp"

namespace mpc::sequencer
{
    class TrackStateView;
    class SequenceStateView;
    struct SequencerState;

    class SequencerStateView
    {
    public:
        explicit SequencerStateView(
            const SequencerState *) noexcept;

        SequenceStateView
            getSequenceState(SequenceIndex) const;

        TrackStateView getTrackState(SequenceIndex,
                                                      TrackIndex) const;

        SequenceIndex getSelectedSequenceIndex() const noexcept;
        SongIndex getSelectedSongIndex() const noexcept;
        SongStepIndex getSelectedSongStepIndex() const noexcept;

        TransportStateView getTransportStateView() const;

        SongStateView getSongStateView(SongIndex) const;

        bool isUndoSequenceAvailable() const noexcept;

        SequenceStateView getSelectedSequenceStateView() const;

    private:
        const SequencerState *const state;
    };
} // namespace mpc::sequencer
