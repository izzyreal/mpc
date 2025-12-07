#include "sequencer/TrackStateHandler.hpp"

#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;

void TrackStateHandler::applyClearEvents(const ClearEvents &m,
                                         SequencerState &state) const
{
    auto &lock = manager->trackLocks[m.sequence][m.track];

    if (!lock.try_acquire())
    {
        manager->enqueue(m);
        return;
    }

    auto &track = state.sequences[m.sequence].tracks[m.track];

    EventData *cur = track.eventsHead;
    while (cur)
    {
        EventData *nxt = cur->next;
        cur->prev = nullptr;
        cur->next = nullptr;
        manager->returnEventToPool(cur);
        cur = nxt;
    }
    track.eventsHead = nullptr;

    lock.release();
}