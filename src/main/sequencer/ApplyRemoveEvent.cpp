#include "sequencer/TrackStateHandler.hpp"

#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;

void TrackStateHandler::applyRemoveEvent(const RemoveEvent &m, SequencerState &state) const
{
    EventData *e = m.handle;

    auto &lock = manager->trackLocks[e->sequenceIndex][e->trackIndex];

    if (!lock.try_acquire())
    {
        manager->enqueue(m);
        return;
    }

    auto &track =
        state.sequences[e->sequenceIndex].tracks[e->trackIndex];

    const EventData * head = track.eventsHead;

    int removedIndex = 0;

    {
        const EventData * it = head;
        int idx = 0;
        while (it && it != e)
        {
            it = it->next;
            idx++;
        }
        removedIndex = idx;
    }

    if (e->prev)
        e->prev->next = e->next;

    if (e->next)
        e->next->prev = e->prev;

    if (track.eventsHead == e)
        track.eventsHead = e->next;

    if (track.playEventIndex > removedIndex)
    {
        --track.playEventIndex;
    }
    else if (track.playEventIndex == removedIndex)
    {
        track.playEventIndex = std::max(EventIndex(0), track.playEventIndex);
    }

    e->prev = nullptr;
    e->next = nullptr;

    manager->returnEventToPool(e);
    lock.release();
}