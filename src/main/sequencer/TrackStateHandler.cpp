#include "sequencer/TrackStateHandler.hpp"

#include "SequencerState.hpp"
#include "SequencerStateManager.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

TrackStateHandler::TrackStateHandler(SequencerStateManager *manager,
                                     Sequencer *sequencer)
    : manager(manager), sequencer(sequencer)
{
}

TrackStateHandler::~TrackStateHandler() {}

void updateEvent(EventData *const e, const EventData &replacement)
{
    auto *prev = e->prev;
    auto *next = e->next;

    *e = replacement;

    e->prev = prev;
    e->next = next;
}

void TrackStateHandler::applyMessage(
    SequencerState &state, std::vector<utils::SimpleAction> &actions,
    const TrackMessage &msg) const
{
    const auto visitor = Overload{
        [&](const SetTrackDeviceIndex &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.device = m.deviceIndex;
        },
        [&](const SetTrackBusType &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.busType = m.busType;
        },
        [&](const SetTrackProgramChange &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.programChange = m.pgm;
        },
        [&](const SetTrackVelocityRatio &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.velocityRatio = m.ratio;
        },
        [&](const SetPlayEventIndex &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.playEventIndex = m.idx;
        },
        [&](const SetTrackOn &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.on = m.on;
        },
        [&](const SetTrackUsed &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.used = m.used;
        },
        [&](const FinalizeNoteEventNonLive &m)
        {
            auto &lock =
                manager
                    ->trackLocks[m.handle->sequenceIndex][m.handle->trackIndex];

            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            EventData *it = state.sequences[m.handle->sequenceIndex]
                                .tracks[m.handle->trackIndex]
                                .head;
            while (it)
            {
                if (it->beingRecorded &&
                    it->noteNumber == m.handle->noteNumber)
                {
                    it->duration = m.duration;
                    it->beingRecorded = false;
                    break;
                }
                it = it->next;
            }

            lock.release();
        },
        [&](const UpdateEvent &m)
        {
            auto &lock =
                manager
                    ->trackLocks[m.handle->sequenceIndex][m.handle->trackIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }
            updateEvent(m.handle, m.snapshot);
            lock.release();
        },
        [&](const InsertAcquiredEvent &m)
        {
            auto &lock =
                manager
                    ->trackLocks[m.handle->sequenceIndex][m.handle->trackIndex];

            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &track = state.sequences[m.handle->sequenceIndex]
                              .tracks[m.handle->trackIndex];

            manager->insertAcquiredEvent(track, m.handle);

            lock.release();

            actions.emplace_back(m.onComplete);
        },
        [&](const ClearEvents &m)
        {
            auto &lock = manager->trackLocks[m.sequence][m.track];

            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &track = state.sequences[m.sequence].tracks[m.track];

            EventData *cur = track.head;
            while (cur)
            {
                EventData *nxt = cur->next;
                cur->prev = nullptr;
                cur->next = nullptr;
                manager->returnEventToPool(cur);
                cur = nxt;
            }
            track.head = nullptr;

            lock.release();
        },
        [&](const RemoveDoubles &m)
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

            EventData *cur = track.head;

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
                            track.head = cur->next;
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
        },
        [&](const UpdateEventTick &m)
        {
            auto &lock =
                manager
                    ->trackLocks[m.handle->sequenceIndex][m.handle->trackIndex];

            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &track = state.sequences[m.handle->sequenceIndex]
                              .tracks[m.handle->trackIndex];

            EventData *e = m.handle;
            const Tick newTick = m.newTick;
            const Tick oldTick = e->tick;

            if (newTick == oldTick)
            {
                lock.release();
                return;
            }

            EventData *&head = track.head;

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
                lock.release();
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
                lock.release();
                return;
            }

            e->prev = prev;
            e->next = cur;
            prev->next = e;
            if (cur)
            {
                cur->prev = e;
            }

            lock.release();
        },
        [&](const RemoveEvent &m)
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

            const EventData * head = track.head;

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

            if (track.head == e)
                track.head = e->next;

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
        },
        [&](const UpdateEvents &m)
        {
            auto &lock = manager->trackLocks[m.sequence][m.track];

            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &track = state.sequences[m.sequence].tracks[m.track];

            EventData *cur = track.head;
            while (cur)
            {
                EventData *nxt = cur->next;
                cur->prev = nullptr;
                cur->next = nullptr;
                manager->returnEventToPool(cur);
                cur = nxt;
            }
            track.head = nullptr;

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
        }};

    std::visit(visitor, msg);
}
