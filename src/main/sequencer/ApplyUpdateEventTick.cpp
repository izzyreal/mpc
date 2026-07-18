#include "sequencer/TrackStateHandler.hpp"

#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;

void TrackStateHandler::applyUpdateEventTick(const UpdateEventTick &m,
                                             SequencerState &state) const
{
    auto &lock =
        manager->trackLocks[m.handle->sequenceIndex][m.handle->trackIndex];

    if (!lock.try_acquire())
    {
        manager->enqueue(m);
        return;
    }

    auto &track =
        state.sequences[m.handle->sequenceIndex].tracks[m.handle->trackIndex];

    updateEventTickLocked(track, m.handle, m.newTick);

    lock.release();
}

void TrackStateHandler::updateEventTickLocked(
    TrackState &track,
    EventData *const e,
    const Tick newTick) const
{
    const Tick oldTick = e->tick;

    if (newTick == oldTick)
    {
        return;
    }

    EventData *&head = track.eventsHead;

    if (e->prev)
    {
        e->prev->next = e->next;
    }
    else
    {
        head = e->next;
    }
    if (e->next)
    {
        e->next->prev = e->prev;
    }

    e->prev = nullptr;
    e->next = nullptr;
    e->tick = newTick;

    if (!head)
    {
        head = e;
        return;
    }

    EventData *cur = head;
    EventData *prev = nullptr;

    while (cur && cur->tick <= newTick)
    {
        prev = cur;
        cur = cur->next;
    }

    if (!prev)
    {
        e->next = head;
        head->prev = e;
        head = e;
        return;
    }

    e->prev = prev;
    e->next = cur;
    prev->next = e;
    if (cur)
    {
        cur->prev = e;
    }
}
