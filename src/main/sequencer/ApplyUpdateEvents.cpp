#include "sequencer/TrackStateHandler.hpp"

#include "sequencer/SequencerStateManager.hpp"

#include <cstring>

using namespace mpc::sequencer;

void TrackStateHandler::applyUpdateEvents(const UpdateEvents &m,
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

    for (const auto &src : m.snapshot)
    {
        EventData *e = manager->acquireEvent();
        std::memcpy(e, &src, sizeof(EventData));
        e->sequenceIndex = m.sequence;
        e->trackIndex = m.track;
        e->prev = nullptr;
        e->next = nullptr;
        manager->insertAcquiredEvent(track, e);
    }

    lock.release();
}
