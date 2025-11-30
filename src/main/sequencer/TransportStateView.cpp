#include "sequencer/TransportStateView.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/TransportState.hpp"

using namespace mpc::sequencer;

TransportStateView::TransportStateView(const TransportState &s) noexcept
    : state(s)
{
}

double TransportStateView::getPositionQuarterNotes() const
{
    return state.positionQuarterNotes;
}

double TransportStateView::getPlayStartPositionQuarterNotes() const
{
    return state.playStartPositionQuarterNotes;
}

int64_t TransportStateView::getPositionTicks() const
{
    return Sequencer::quarterNotesToTicks(state.positionQuarterNotes);
}

bool TransportStateView::isSequencerRunning() const
{
    return state.sequencerRunning;
}

bool TransportStateView::isCountEnabled() const
{
    return state.countEnabled;
}

bool TransportStateView::isRecording() const
{
    return state.recording;
}

bool TransportStateView::isOverdubbing() const
{
    return state.overdubbing;
}

bool TransportStateView::isRecordingOrOverdubbing() const
{
    return state.recording || state.overdubbing;
}

bool TransportStateView::isCountingIn() const
{
    return state.countingIn;
}
