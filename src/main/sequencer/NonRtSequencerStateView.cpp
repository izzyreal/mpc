#include "sequencer/NonRtSequencerStateView.hpp"

#include "NonRtSequenceStateView.hpp"
#include "NonRtTrackStateView.hpp"
#include "Sequencer.hpp"
#include "sequencer/NonRtSequencerState.hpp"

using namespace mpc::sequencer;

NonRtSequencerStateView::NonRtSequencerStateView(
    const std::shared_ptr<const NonRtSequencerState> &s) noexcept
    : state(s)
{
}

PlaybackState NonRtSequencerStateView::getPlaybackState() const
{
    return state->playbackState;
}

std::shared_ptr<NonRtSequenceStateView>
NonRtSequencerStateView::getNonRtSequenceState(const SequenceIndex i) const
{
    return std::make_shared<NonRtSequenceStateView>(state->sequences[i]);
}

std::shared_ptr<NonRtTrackStateView>
NonRtSequencerStateView::getNonRtTrackState(const SequenceIndex sequenceIndex,
                                            const TrackIndex trackIndex) const
{
    return std::make_shared<NonRtTrackStateView>(state->sequences[sequenceIndex].tracks[trackIndex]);
}

mpc::SequenceIndex NonRtSequencerStateView::getSelectedSequenceIndex() const noexcept
{
    return state->selectedSequenceIndex;
}

double NonRtSequencerStateView::getPositionQuarterNotes() const
{
    return state->transportState.positionQuarterNotes;
}

double NonRtSequencerStateView::getPlayStartPositionQuarterNotes() const
{
    return state->transportState.playStartPositionQuarterNotes;
}

int64_t NonRtSequencerStateView::getPositionTicks() const
{
    return Sequencer::quarterNotesToTicks(
        state->transportState.positionQuarterNotes);
}

bool NonRtSequencerStateView::isSequencerRunning() const
{
    return state->transportState.sequencerRunning;
}
