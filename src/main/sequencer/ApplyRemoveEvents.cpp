#include "sequencer/TrackStateHandler.hpp"

#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;

void TrackStateHandler::applyRemoveEvents(const RemoveEvents &m,
                                         SequencerState &state) const
{
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
}