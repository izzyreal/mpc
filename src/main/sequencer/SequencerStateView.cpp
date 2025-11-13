#include "sequencer/SequencerStateView.hpp"

#include "sequencer/SequencerState.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

SequencerStateView::SequencerStateView(
    const std::shared_ptr<const SequencerState> &s) noexcept
    : state(s)
{
}

double SequencerStateView::getPositionQuarterNotes() const
{
    return state->transportState.positionQuarterNotes;
}

double SequencerStateView::getPlayStartPositionQuarterNotes() const
{
    return state->transportState.playStartPositionQuarterNotes;
}

int64_t SequencerStateView::getPositionTicks() const
{
    return Sequencer::quarterNotesToTicks(state->transportState.positionQuarterNotes);
}
