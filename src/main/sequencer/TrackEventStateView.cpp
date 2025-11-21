#include "sequencer/TrackEventStateView.hpp"

#include "sequencer/TrackEventState.hpp"

using namespace mpc::sequencer;

TrackEventStateView::TrackEventStateView(
    const std::shared_ptr<const TrackEventState> &s) noexcept
    : state(s)
{
}
