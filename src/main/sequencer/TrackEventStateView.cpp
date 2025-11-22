#include "sequencer/TrackEventStateView.hpp"

#include "sequencer/TrackEventState.hpp"

using namespace mpc::sequencer;

TrackEventStateView::TrackEventStateView(
    const std::shared_ptr<const TrackEventState> &s) noexcept
    : state(s)
{
}

std::optional<std::pair<mpc::EventIndex, EventState>>
TrackEventStateView::findNoteEvent(const int tick, const NoteNumber note) const
{
    for (int i = 0; i < state->events.size(); ++i)
    {
        if (const auto &e = state->events[i]; e.type == EventType::NoteOn &&
                                              e.tick == tick &&
                                              e.noteNumber == note)
        {
            return {{EventIndex(i), e}};
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

std::vector<std::pair<mpc::EventIndex, EventState>>
TrackEventStateView::getEventRange(const int startTick, const int endTick) const
{
    std::vector<std::pair<EventIndex, EventState>> result;

    for (int i = 0; i < state->events.size(); ++i)
    {
        auto &e = state->events[i];
        if (e.tick > endTick)
        {
            break;
        }
        if (e.tick >= startTick && e.tick <= endTick)
        {
            result.push_back({EventIndex(i), e});
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

std::vector<std::pair<mpc::EventIndex, EventState>> TrackEventStateView::getNoteEvents() const
{
    std::vector<std::pair<EventIndex, EventState>> result;

    for (int i = 0; i < state->events.size(); ++i)
    {
        if (auto &e = state->events[i]; e.type == EventType::NoteOn)
        {
            result.push_back({EventIndex(i), e});
        }
    }

    return result;
}

std::pair<mpc::EventIndex, EventState> TrackEventStateView::findRecordingNoteOnByNoteNumber(
    const NoteNumber noteNumber) const
{
    for (auto &e : getNoteEvents())
    {
        if (e.second.noteNumber == noteNumber &&
            e.second.beingRecorded)
        {
            return e;
        }
    }
    return {};
}

std::pair<mpc::EventIndex, EventState> TrackEventStateView::findRecordingNoteOnByNoteEventId(
    const NoteEventId id) const
{
    for (auto &e : getNoteEvents())
    {
        if (e.second.noteEventId == id &&
            e.second.beingRecorded)
        {
            return e;
        }
    }
    return {};
}
