#include "sequencer/SequenceStateView.hpp"

#include "sequencer/SequenceState.hpp"

using namespace mpc::sequencer;

SequenceStateView::SequenceStateView(
    const std::shared_ptr<const SequenceState> &s) noexcept
    : state(s)
{
}

mpc::Tick SequenceStateView::getBarLength(const int barIndex) const
{
    return state->barLengths[barIndex];
}

std::array<mpc::Tick, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
SequenceStateView::getBarLengths() const
{
    return state->barLengths;
}
