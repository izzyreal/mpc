#include "sequencer/SongStateView.hpp"

#include "sequencer/SongState.hpp"

using namespace mpc::sequencer;

SongStateView::SongStateView(const SongState &s) noexcept : state(s) {}

bool SongStateView::isLoopEnabled() const
{
    return state.loopEnabled;
}

mpc::SongStepIndex SongStateView::getFirstLoopStepIndex() const
{
    return state.firstLoopStepIndex;
}

mpc::SongStepIndex SongStateView::getLastLoopStepIndex() const
{
    return state.lastLoopStepIndex;
}

bool SongStateView::isUsed() const
{
    return state.used;
}

SongStepState SongStateView::getStep(const SongStepIndex idx) const
{
    return state.steps[idx];
}

uint8_t SongStateView::getStepCount()
{
    const auto it = std::find_if(state.steps.begin(), state.steps.end(),
                                 [&](const auto &s)
                                 {
                                     return s.sequenceIndex == NoSequenceIndex;
                                 });

    return it == state.steps.end() ? state.steps.size()
                                   : std::distance(state.steps.begin(), it);
}