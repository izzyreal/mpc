#include "sequencer/TrackStateView.hpp"

#include "EventState.hpp"
#include "TrackState.hpp"

using namespace mpc::sequencer;

TrackStateView::TrackStateView(const TrackState &s) noexcept : state(s) {}

EventState *TrackStateView::findNoteEvent(const int tick,
                                          const NoteNumber note) const
{
    const auto noteEvents = getNoteEvents();

    for (const auto e : noteEvents)
    {
        if (e->tick == tick && e->noteNumber == note)
        {
            return e;
        }
    }

    return nullptr;
}

EventState *TrackStateView::getEventByIndex(const EventIndex idx) const
{
    int counter = 0;

    EventState *it = state.head;

    while (it)
    {
        if (counter++ == idx)
        {
            return it;
        }
        it = it->next;
    }

    return nullptr;
}

std::vector<EventState *> TrackStateView::getEventRange(const int startTick,
                                                        const int endTick) const
{
    std::vector<EventState *> result;

    EventState *it = state.head;

    while (it)
    {
        it = it->next;
        if (it->tick > endTick)
        {
            break;
        }
        if (it->tick >= startTick && it->tick <= endTick)
        {
            result.push_back(it);
        }
    }

    return result;
}

bool TrackStateView::isEventsEmpty() const
{
    return state.head == nullptr;
}

int TrackStateView::getEventCount() const
{
    int result = 0;

    const EventState *it = state.head;

    while (it)
    {
        result++;
        it = it->next;
    }
    return result;
}

std::vector<EventState *> TrackStateView::getNoteEvents() const
{
    std::vector<EventState *> result;

    EventState *it = state.head;

    while (it)
    {
        if (it->type == EventType::NoteOn)
        {
            result.push_back(it);
        }

        it = it->next;
    }

    return result;
}

std::vector<EventState *> TrackStateView::getEvents() const
{
    std::vector<EventState *> result;

    EventState *it = state.head;

    while (it)
    {
        result.push_back(it);
        it = it->next;
    }

    return result;
}

EventState *TrackStateView::findRecordingNoteOnByNoteNumber(
    const NoteNumber noteNumber) const
{
    for (const auto &e : getNoteEvents())
    {
        if (e->noteNumber == noteNumber && e->beingRecorded)
        {
            return e;
        }
    }
    return nullptr;
}

EventState *
TrackStateView::findRecordingNoteOn(const EventState *eventState) const
{
    for (const auto &e : getNoteEvents())
    {
        if (e == eventState && e->beingRecorded)
        {
            return e;
        }
    }

    return nullptr;
}

EventState *TrackStateView::getEvent(const EventState *eventState) const
{
    EventState *it = state.head;

    while (it)
    {
        if (it == eventState)
        {
            return it;
        }
        it = it->next;
    }

    return nullptr;
}

mpc::EventIndex TrackStateView::getPlayEventIndex() const
{
    return state.playEventIndex;
}

uint8_t TrackStateView::getVelocityRatio() const
{
    return state.velocityRatio;
}

uint8_t TrackStateView::getProgramChange() const
{
    return state.programChange;
}

uint8_t TrackStateView::getDeviceIndex() const
{
    return state.device;
}

BusType TrackStateView::getBusType() const
{
    return state.busType;
}

bool TrackStateView::isOn() const
{
    return state.on;
}

bool TrackStateView::isUsed() const
{
    return state.used;
}
