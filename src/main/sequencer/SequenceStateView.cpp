#include "sequencer/SequenceStateView.hpp"

#include "TrackStateView.hpp"
#include "Sequencer.hpp"
#include "sequencer/SequencerState.hpp"

using namespace mpc::sequencer;

SequenceStateView::SequenceStateView(const SequenceState &s) noexcept : state(s)
{
}

std::string_view SequenceStateView::getName() const
{
    return state.name;
}

std::shared_ptr<TrackStateView>
SequenceStateView::getTrack(const int trackIndex) const
{
    return std::make_shared<TrackStateView>(state.tracks[trackIndex]);
}

mpc::Tick SequenceStateView::getBarLength(const int barIndex) const
{
    return state.barLengths[barIndex];
}

std::array<mpc::Tick, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
SequenceStateView::getBarLengths() const
{
    return state.barLengths;
}

std::array<TimeSignature, mpc::Mpc2000XlSpecs::MAX_BAR_COUNT>
SequenceStateView::getTimeSignatures() const
{
    return state.timeSignatures;
}

TimeSignature SequenceStateView::getTimeSignature(const int barIndex) const
{
    return state.timeSignatures[barIndex];
}

mpc::BarIndex SequenceStateView::getLastBarIndex() const
{
    return state.lastBarIndex;
}

int SequenceStateView::getBarCount() const
{
    return getLastBarIndex() + 1;
}

mpc::Tick SequenceStateView::getFirstTickOfBar(const BarIndex barIndex) const
{
    int res = 0;

    for (int i = 0; i < barIndex; i++)
    {
        res += getBarLength(i);
    }

    return res;
}

mpc::Tick SequenceStateView::getLastTick() const
{
    int lastTick = 0;

    for (int i = 0; i < getBarCount(); i++)
    {
        lastTick += getBarLength(i);
    }

    return lastTick;
}

double SequenceStateView::getInitialTempo() const
{
    return state.initialTempo;
}

mpc::Tick SequenceStateView::getLoopStartTick() const
{
    return getFirstTickOfBar(state.firstLoopBarIndex);
}

mpc::Tick SequenceStateView::getLoopEndTick() const
{
    if (state.lastLoopBarIndex == EndOfSequence)
    {
        return getLastTick();
    }

    return getFirstTickOfBar(state.lastLoopBarIndex + 1);
}

mpc::BarIndex SequenceStateView::getFirstLoopBarIndex() const
{
    return state.firstLoopBarIndex;
}

mpc::BarIndex SequenceStateView::getLastLoopBarIndex() const
{
    return state.lastLoopBarIndex;
}

bool SequenceStateView::isLoopEnabled() const
{
    return state.loopEnabled;
}

bool SequenceStateView::isUsed() const
{
    return state.used;
}

bool SequenceStateView::isTempoChangeEnabled() const
{
    return state.tempoChangeEnabled;
}

std::vector<EventData> SequenceStateView::getTempoChangeEvents() const
{
    std::vector<EventData> result;
    const EventData *it = state.tracks[TempoChangeTrackIndex].eventsHead;
    while (it)
    {
        result.push_back(*it);
        it = it->next;
    }

    return result;
}
