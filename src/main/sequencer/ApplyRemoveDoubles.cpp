#include "sequencer/TrackStateHandler.hpp"

#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::sequencer;

void TrackStateHandler::applyRemoveDoubles(const RemoveDoubles &m,
                                           SequencerState &state) const
{
    auto &lock = manager->trackLocks[m.sequence][m.track];

    if (!lock.try_acquire())
    {
        manager->enqueue(m);
        return;
    }

    auto &track = state.sequences[m.sequence].tracks[m.track];

    std::vector<NoteNumber> notesAtTick;
    Tick lastTick = -100;

    EventData *cur = track.eventsHead;

    while (cur)
    {
        EventData *next = cur->next;

        if (cur->type == EventType::NoteOn)
        {
            if (cur->tick != lastTick)
            {
                notesAtTick.clear();
            }

            bool exists = false;
            for (auto n : notesAtTick)
            {
                if (n == cur->noteNumber)
                {
                    exists = true;
                }
            }

            if (!exists)
            {
                notesAtTick.push_back(cur->noteNumber);
                lastTick = cur->tick;
            }
            else
            {
                if (cur->prev)
                {
                    cur->prev->next = cur->next;
                }
                else
                {
                    track.eventsHead = cur->next;
                }
                if (cur->next)
                {
                    cur->next->prev = cur->prev;
                }

                cur->prev = nullptr;
                cur->next = nullptr;

                manager->returnEventToPool(cur);
            }
        }

        cur = next;
    }

    lock.release();
}