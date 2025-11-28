#include "sequencer/SequencerStateView.hpp"

#include "SequenceStateView.hpp"
#include "TrackStateView.hpp"
#include "Sequencer.hpp"
#include "sequencer/SequencerState.hpp"

using namespace mpc::sequencer;

SequencerStateView::SequencerStateView(
    const std::shared_ptr<const SequencerState> &s) noexcept
    : state(s)
{
}

PlaybackState SequencerStateView::getPlaybackState() const
{
    return state->playbackState;
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

double SequencerStateView::getPositionQuarterNotes() const
{
    return state->transport.positionQuarterNotes;
}

double SequencerStateView::getPlayStartPositionQuarterNotes() const
{
    return state->transport.playStartPositionQuarterNotes;
}

int64_t SequencerStateView::getPositionTicks() const
{
    return Sequencer::quarterNotesToTicks(
        state->transport.positionQuarterNotes);
}

bool SequencerStateView::isSequencerRunning() const
{
    return state->transport.sequencerRunning;
}
TransportState SequencerStateView::getTransportState() const
{
    return state->transport;
}
