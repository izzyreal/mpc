#include "sequencer/SequencerStateView.hpp"

#include "SequencerState.hpp"

using namespace mpc::sequencer;

SequencerStateView::SequencerStateView(
    const std::shared_ptr<const SequencerState> &s) noexcept
    : state(s)
{
}

mpc::TimeInSamples SequencerStateView::getTimeInSamples() const noexcept
{
    return state->timeInSamples;
}
