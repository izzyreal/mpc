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

void TrackStateHandler::applyMessage(SequencerState &state,
                                     const TrackMessage &msg) const
{
    const auto visitor = Overload{
        [&](const SetTrackDeviceIndex &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            track.deviceIndex = m.deviceIndex;
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
                                .eventsHead;
            while (it)
            {
                if (it->beingRecorded && it->noteNumber == m.handle->noteNumber)
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
        },
        [&](const RemoveEvents &m)
        {
            auto &lock = manager->trackLocks[m.sequence][m.track];

            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            applyRemoveEvents(m, state);

            lock.release();
        },
        [&](const RemoveDoubles &m)
        {
            applyRemoveDoubles(m, state);
        },
        [&](const UpdateEventTick &m)
        {
            applyUpdateEventTick(m, state);
        },
        [&](const RemoveEvent &m)
        {
            applyRemoveEvent(m, state);
        },
        [&](const SetTrackTransmitProgramChangesEnabled &m)
        {
            state.sequences[m.sequenceIndex]
                .tracks[m.trackIndex]
                .transmitProgramChangesEnabled = m.enabled;
        },
        [&](const SetTrackName &m)
        {
            state.sequences[m.sequenceIndex].tracks[m.trackIndex].name.assign(
                m.name);
        }};

    std::visit(visitor, msg);
}
