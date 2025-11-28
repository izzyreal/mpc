#pragma once

#include "SequencerState.hpp"
#include "PlaybackState.hpp"

#include <memory>
#include <vector>

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

        PlaybackState getPlaybackState() const;

        std::shared_ptr<SequenceStateView>
            getSequenceState(SequenceIndex) const;

        std::shared_ptr<TrackStateView> getTrackState(SequenceIndex,
                                                      TrackIndex) const;

        SequenceIndex getSelectedSequenceIndex() const noexcept;

        double getPositionQuarterNotes() const;

        double getPlayStartPositionQuarterNotes() const;

        int64_t getPositionTicks() const;

        bool isSequencerRunning() const;

        TransportState getTransportState() const;

    private:
        const std::shared_ptr<const SequencerState> state;
    };
} // namespace mpc::sequencer
