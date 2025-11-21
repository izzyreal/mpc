#include "sequencer/SequenceStateView.hpp"

#include "sequencer/SequenceState.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::sequencer;

SequenceStateView::SequenceStateView(
    const std::shared_ptr<const SequenceState> &s) noexcept
    : state(s)
{
}
