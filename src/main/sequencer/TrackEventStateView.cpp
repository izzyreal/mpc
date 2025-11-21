#include "sequencer/TrackEventStateView.hpp"

#include "sequencer/TrackEventState.hpp"

using namespace mpc::sequencer;

TrackEventStateView::TrackEventStateView(
    const std::shared_ptr<const TrackEventState> &s) noexcept
    : state(s)
{
}

std::optional<EventState>
TrackEventStateView::findNoteEvent(const int tick, const NoteNumber note) const
{
    for (auto &e : state->events)
    {
        if (e.type == EventType::NoteOn && e.tick == tick &&
            e.noteNumber == note)
        {
            return e;
        }
    }

    return std::nullopt;
}

EventState TrackEventStateView::getEventByIndex(const EventIndex idx) const
{
    if (idx >= 0 && idx < state->events.size())
    {
        return state->events[idx];
    }
    return {};
}

std::vector<EventState>
TrackEventStateView::getEventRange(const int startTick, const int endTick) const
{
    std::vector<EventState> result;

    for (auto &e : state->events)
    {
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

bool TrackEventStateView::isEventsEmpty() const
{
    return state->events.empty();
}

int TrackEventStateView::getEventCount() const
{
    return state->events.size();
}

std::vector<EventState> TrackEventStateView::getNoteEvents() const
{
    std::vector<EventState> result;

    for (auto &e : state->events)
    {
        if (e.type == EventType::NoteOn)
        {
            result.push_back(e);
        }
    }

    return result;
}

EventState TrackEventStateView::findRecordingNoteOnByNoteNumber(
    const NoteNumber noteNumber) const
{
    for (auto &e : getNoteEvents())
    {
        if (e.noteNumber == noteNumber &&
            e.beingRecorded)
        {
            return e;
        }
    }
    return {};
}

EventState TrackEventStateView::findRecordingNoteOnByNoteEventId(
    const NoteEventId id) const
{
    for (auto &e : getNoteEvents())
    {
        if (e.noteEventId == id &&
            e.beingRecorded)
        {
            return e;
        }
    }
    return {};
}
