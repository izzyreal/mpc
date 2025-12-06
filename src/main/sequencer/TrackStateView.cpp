#include "sequencer/TrackStateView.hpp"

#include "EventData.hpp"
#include "TrackState.hpp"

using namespace mpc::sequencer;

TrackStateView::TrackStateView(const TrackState &s) noexcept : state(s) {}

EventData *TrackStateView::findNoteEvent(const int tick,
                                         const NoteNumber note) const
{
    EventData *it = state.eventsHead;

    while (it)
    {
        if (it->type == EventType::NoteOn && it->tick == tick &&
            it->noteNumber == note)
        {
            return it;
        }

        it = it->next;
    }

    return nullptr;
}

EventData *TrackStateView::getEventByIndex(const EventIndex idx) const
{
    int counter = 0;

    EventData *it = state.eventsHead;

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

std::vector<EventData *> TrackStateView::getEventRange(const int startTick,
                                                       const int endTick) const
{
    std::vector<EventData *> result;

    EventData *it = state.eventsHead;

    while (it)
    {
        if (it->tick > endTick)
        {
            break;
        }
        if (it->tick >= startTick && it->tick < endTick)
        {
            result.push_back(it);
        }
        it = it->next;
    }

    return result;
}

bool TrackStateView::isEventsEmpty() const
{
    return state.eventsHead == nullptr;
}

int TrackStateView::getEventCount() const
{
    int result = 0;

    const EventData *it = state.eventsHead;

    while (it)
    {
        result++;
        it = it->next;
    }
    return result;
}

std::vector<EventData *> TrackStateView::getNoteEvents() const
{
    std::vector<EventData *> result;

    EventData *it = state.eventsHead;

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

std::vector<EventData *> TrackStateView::getEvents() const
{
    std::vector<EventData *> result;

    EventData *it = state.eventsHead;

    while (it)
    {
        result.push_back(it);
        it = it->next;
    }

    return result;
}

EventData *TrackStateView::findRecordingNoteOnByNoteNumber(
    const NoteNumber noteNumber) const
{
    EventData *it = state.eventsHead;

    while (it)
    {
        if (it->type == EventType::NoteOn && it->noteNumber == noteNumber &&
            it->beingRecorded)
        {
            return it;
        }

        it = it->next;
    }

    return nullptr;
}

EventData *
TrackStateView::findRecordingNoteOn(const EventData *eventState) const
{
    assert(eventState->type == EventType::NoteOn);

    EventData *it = state.eventsHead;

    while (it)
    {
        if (it->type == EventType::NoteOn && it == eventState &&
            it->beingRecorded)
        {
            return it;
        }

        it = it->next;
    }

    return nullptr;
}

EventData *TrackStateView::getEvent(const EventData *eventState) const
{
    EventData *it = state.eventsHead;

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
