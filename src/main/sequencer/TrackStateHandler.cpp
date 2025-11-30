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

void updateEvent(EventState* e, const EventState& replacement)
{
    // Preserve links
    auto* prev = e->prev;
    auto* next = e->next;

    // Copy everything else (POD-style)
    *e = replacement;

    // Restore links
    e->prev = prev;
    e->next = next;
}

void TrackStateHandler::applyMessage(
    SequencerState &state, std::vector<std::function<void()>> &actions,
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
        [&](const UpdateTrackIndexOfAllEvents &m)
        {
            EventState *it = state.sequences[m.sequence].tracks[m.trackIndex].head;
            while (it)
            {
                it->trackIndex = m.trackIndex;
                it = it->next;
            }
        },
        [&](const FinalizeNonLiveNoteEvent &m)
        {
            EventState *it = state.sequences[m.noteOnEvent->sequenceIndex]
                               .tracks[m.noteOnEvent->trackIndex]
                               .head;
            while (it)
            {
                if (it->beingRecorded && it->duration == NoDuration &&
                    it->noteNumber == m.noteOnEvent->noteNumber)
                {
                    it->duration = m.duration;
                    it->beingRecorded = false;
                    break;
                }
                it = it->next;
            }
        },
        [&](const UpdateEvent &m)
        {
            updateEvent(m.event, m.payload);
        },
        [&](const InsertEvent &m)
        {
            auto &track = state.sequences[m.eventState->sequenceIndex]
                               .tracks[m.eventState->trackIndex];

            manager->insertEvent(track, m.eventState, m.allowMultipleNoteEventsWithSameNoteOnSameTick);

            actions.push_back(m.onComplete);
        },
        [&](const ClearEvents &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];

            EventState* cur = track.head;
            while (cur)
            {
                EventState* nxt = cur->next;
                cur->prev = nullptr;
                cur->next = nullptr;
                manager->returnEventToPool(cur);
                cur = nxt;
            }
            track.head = nullptr;
        },
        [&](const RemoveDoubles &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];

            std::vector<NoteNumber> notesAtTick;
            Tick lastTick = -100;

            EventState* cur = track.head;

            while (cur)
            {
                EventState* next = cur->next;

                if (cur->type == EventType::NoteOn)
                {
                    if (cur->tick != lastTick)
                        notesAtTick.clear();

                    bool exists = false;
                    for (auto n : notesAtTick)
                        if (n == cur->noteNumber)
                            exists = true;

                    if (!exists)
                    {
                        notesAtTick.push_back(cur->noteNumber);
                        lastTick = cur->tick;
                    }
                    else
                    {
                        manager->freeEvent(track.head, cur);
                    }
                }

                cur = next;
            }
        },
        [&](const UpdateEventTick &m)
        {
            auto &track = state.sequences[m.eventState->sequenceIndex]
                              .tracks[m.eventState->trackIndex];

            EventState* e = m.eventState;
            const Tick newTick = m.newTick;
            const Tick oldTick = e->tick;

            if (newTick == oldTick)
                return;

            EventState*& head = track.head;

            if (e->prev) e->prev->next = e->next;
            else head = e->next;
            if (e->next) e->next->prev = e->prev;

            e->prev = nullptr;
            e->next = nullptr;
            e->tick = newTick;

            if (!head) { head = e; return; }

            EventState* cur = head;
            EventState* prev = nullptr;

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
            if (cur) cur->prev = e;
        },
        [&](const RemoveEvent &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            EventState* e = m.eventState;
            if (!e) return;

            if (e->prev) e->prev->next = e->next;
            if (e->next) e->next->prev = e->prev;
            if (track.head == e) track.head = e->next;

            e->prev = nullptr;
            e->next = nullptr;

            manager->returnEventToPool(e);
        },
        [&](const UpdateEvents& m)
        {
            auto& track = state.sequences[m.sequence].tracks[m.track];

            EventState* cur = track.head;
            while (cur) {
                EventState* nxt = cur->next;
                cur->prev = nullptr;
                cur->next = nullptr;
                manager->freeEvent(track.head, cur);
                cur = nxt;
            }
            track.head = nullptr;

            for (const auto& src : m.snapshot) {
                EventState* e = manager->acquireEvent();
                std::memcpy(e, &src, sizeof(EventState));
                e->prev = nullptr;
                e->next = nullptr;
                manager->insertEvent(track, e, true);
            }
        }};

    std::visit(visitor, msg);
}
