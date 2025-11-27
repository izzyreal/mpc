#pragma once

#include "NonRtSequencerState.hpp"
#include "PlaybackState.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class NonRtTrackStateView;
    class NonRtSequenceStateView;

    struct NonRtSequencerState;

    class NonRtSequencerStateView
    {
    public:
        explicit NonRtSequencerStateView(
            const std::shared_ptr<const NonRtSequencerState> &s) noexcept;

        PlaybackState getPlaybackState() const;

        std::shared_ptr<NonRtSequenceStateView> getNonRtSequenceState(SequenceIndex) const;

        std::shared_ptr<NonRtTrackStateView> getNonRtTrackState(SequenceIndex, TrackIndex) const;

        SequenceIndex getSelectedSequenceIndex() const noexcept;

        double getPositionQuarterNotes() const;

        double getPlayStartPositionQuarterNotes() const;

        int64_t getPositionTicks() const;

        bool isSequencerRunning() const;

        TransportState getTransportState() const;

    private:
        const std::shared_ptr<const NonRtSequencerState> state;
    };
} // namespace mpc::sequencer
