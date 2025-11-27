#include "sequencer/NonRtSequenceStateView.hpp"

#include "NonRtTrackStateView.hpp"
#include "Sequencer.hpp"
#include "sequencer/NonRtSequencerState.hpp"

using namespace mpc::sequencer;

NonRtSequenceStateView::NonRtSequenceStateView(
    const NonRtSequenceState &s) noexcept
: state(s)
{
}

std::shared_ptr<NonRtTrackStateView>
NonRtSequenceStateView::getTrack(const int trackIndex) const
{
    return std::make_shared<NonRtTrackStateView>(state.tracks[trackIndex]);
}

mpc::Tick NonRtSequenceStateView::getBarLength(const int barIndex) const
{
    return state.barLengths[barIndex];
}

std::array<mpc::Tick, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
NonRtSequenceStateView::getBarLengths() const
{
    return state.barLengths;
}

std::array<TimeSignature, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
NonRtSequenceStateView::getTimeSignatures() const
{
    return state.timeSignatures;
}

TimeSignature NonRtSequenceStateView::getTimeSignature(const int barIndex) const
{
    return state.timeSignatures[barIndex];
}

mpc::BarIndex NonRtSequenceStateView::getLastBarIndex() const {
    return state.lastBarIndex;
}

int NonRtSequenceStateView::getBarCount() const
{
    return getLastBarIndex() + 1;
}

mpc::Tick NonRtSequenceStateView::getFirstTickOfBar(const BarIndex barIndex) const
{
    int res = 0;

    for (int i = 0; i < barIndex; i++)
    {
        res += getBarLength(i);
    }

    return res;
}

mpc::Tick NonRtSequenceStateView::getLastTick() const
{
    int lastTick = 0;

    for (int i = 0; i < getBarCount(); i++)
    {
        lastTick += getBarLength(i);
    }

    return lastTick;
}
