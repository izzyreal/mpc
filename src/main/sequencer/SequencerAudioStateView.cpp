#include "sequencer/SequencerAudioStateView.hpp"

#include "SequencerAudioState.hpp"

using namespace mpc::sequencer;

SequencerAudioStateView::SequencerAudioStateView(
    const std::shared_ptr<const SequencerAudioState> &s) noexcept
    : state(s)
{
}

mpc::TimeInSamples SequencerAudioStateView::getTimeInSamples() const noexcept
{
    return state->timeInSamples;
}
