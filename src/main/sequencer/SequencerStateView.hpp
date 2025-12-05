#pragma once

#include "sequencer/SongStateView.hpp"
#include "sequencer/TransportStateView.hpp"
#include "sequencer/SequencerState.hpp"

#include <memory>

namespace mpc::sequencer
{
    class TrackStateView;
    class SequenceStateView;
    struct SequencerState;

    class SequencerStateView
    {
    public:
        explicit SequencerStateView(
            const std::shared_ptr<const SequencerState> &s) noexcept;

        std::shared_ptr<SequenceStateView>
            getSequenceState(SequenceIndex) const;

        std::shared_ptr<TrackStateView> getTrackState(SequenceIndex,
                                                      TrackIndex) const;

        SequenceIndex getSelectedSequenceIndex() const noexcept;
        SongIndex getSelectedSongIndex() const noexcept;
        SongStepIndex getSelectedSongStepIndex() const noexcept;

        TransportStateView getTransportStateView() const;

        SongStateView getSongStateView(SongIndex) const;

    private:
        const std::shared_ptr<const SequencerState> state;
    };
} // namespace mpc::sequencer
