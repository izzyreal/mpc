#include "sequencer/SequencerStateView.hpp"

#include "SequenceStateView.hpp"
#include "TrackStateView.hpp"
#include "TransportStateView.hpp"
#include "sequencer/SequencerState.hpp"

using namespace mpc::sequencer;

SequencerStateView::SequencerStateView(
    const SequencerState *const s) noexcept
    : state(s)
{
}

SequenceStateView
SequencerStateView::getSequenceState(const SequenceIndex i) const
{
    return SequenceStateView(state->sequences[i]);
}

TrackStateView
SequencerStateView::getTrackState(const SequenceIndex sequenceIndex,
                                  const TrackIndex trackIndex) const
{
    return TrackStateView(
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

bool SequencerStateView::isUndoSequenceAvailable() const noexcept
{
    return state->undoSequenceAvailable;
}
