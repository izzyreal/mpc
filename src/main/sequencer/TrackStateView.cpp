#include "sequencer/TrackStateView.hpp"

#include "Sequencer.hpp"
#include "sequencer/SequencerState.hpp"

using namespace mpc::sequencer;

TrackStateView::TrackStateView(const TrackState &s) noexcept
: state(s)
{
}

std::optional<EventState>
TrackStateView::findNoteEvent(const int tick, const NoteNumber note) const
{
    for (int i = 0; i < state.events.size(); ++i)
    {
        if (const auto &e = state.events[i]; e.type == EventType::NoteOn &&
                                              e.tick == tick &&
                                              e.noteNumber == note)
        {
            return e;
        }
    }

    return std::nullopt;
}

EventState TrackStateView::getEventByIndex(const EventIndex idx) const
{
    if (idx >= 0 && idx < state.events.size())
    {
        return state.events[idx];
    }
    return {};
}

std::vector<EventState>
TrackStateView::getEventRange(const int startTick, const int endTick) const
{
    std::vector<EventState> result;

    for (int i = 0; i < state.events.size(); ++i)
    {
        auto &e = state.events[i];
        if (e.tick > endTick)
        {
            break;
        }
        if (e.tick >= startTick && e.tick <= endTick)
        {
            result.push_back(e);
        }
    }

    return result;
}

bool TrackStateView::isEventsEmpty() const
{
    return state.events.empty();
}

int TrackStateView::getEventCount() const
{
    return state.events.size();
}

std::vector<EventState> TrackStateView::getNoteEvents() const
{
    std::vector<EventState> result;

    for (int i = 0; i < state.events.size(); ++i)
    {
        if (auto &e = state.events[i]; e.type == EventType::NoteOn)
        {
            result.push_back(e);
        }
    }

    return result;
}

std::vector<EventState> TrackStateView::getEvents() const {
    return state.events;
}

EventState TrackStateView::findRecordingNoteOnByNoteNumber(
    const NoteNumber noteNumber) const
{
    for (auto &e : getNoteEvents())
    {
        if (e.noteNumber == noteNumber && e.beingRecorded)
        {
            return e;
        }
    }
    return {};
}

EventState TrackStateView::findRecordingNoteOnByNoteEventId(
    const NoteEventId id) const
{
    for (const auto &e : getNoteEvents())
    {
        if (e.noteEventId == id && e.beingRecorded)
        {
            return e;
        }
    }
    return {};
}

EventState TrackStateView::getEventById(const EventId eventId) const
{
    for (auto &e : state.events)
    {
        if (e.eventId == eventId)
        {
            return e;
        }
    }

    return {};
}
