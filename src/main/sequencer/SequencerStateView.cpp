#include "sequencer/SequencerStateView.hpp"

#include "SequenceStateView.hpp"
#include "TrackStateView.hpp"
#include "Sequencer.hpp"
#include "TransportStateView.hpp"
#include "sequencer/SequencerState.hpp"

using namespace mpc::sequencer;

SequencerStateView::SequencerStateView(
    const std::shared_ptr<const SequencerState> &s) noexcept
    : state(s)
{
}

std::shared_ptr<SequenceStateView>
SequencerStateView::getSequenceState(const SequenceIndex i) const
{
    return std::make_shared<SequenceStateView>(state->sequences[i]);
}

std::shared_ptr<TrackStateView>
SequencerStateView::getTrackState(const SequenceIndex sequenceIndex,
                                  const TrackIndex trackIndex) const
{
    return std::make_shared<TrackStateView>(
        state->sequences[sequenceIndex].tracks[trackIndex]);
}

mpc::SequenceIndex SequencerStateView::getSelectedSequenceIndex() const noexcept
{
    return state->selectedSequenceIndex;
}

mpc::SongIndex SequencerStateView::getSelectedSongIndex() const noexcept
{
    return state->selectedSongIndex;
}

mpc::SongStepIndex SequencerStateView::getSelectedSongStepIndex() const noexcept
{
    return state->selectedSongStepIndex;
}

TransportStateView SequencerStateView::getTransportStateView() const
{
    return TransportStateView(state->transport);
}

SongStateView SequencerStateView::getSongStateView(const SongIndex idx) const
{
    return SongStateView(state->songs[idx]);
}
