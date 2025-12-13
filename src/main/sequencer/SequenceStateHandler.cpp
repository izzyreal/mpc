#include "sequencer/SequenceStateHandler.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/TrackStateHandler.hpp"
#include "sequencer/SequencerState.hpp"
#include "utils/VariantUtils.hpp"

#include <cstring>

using namespace mpc::sequencer;

SequenceStateHandler::SequenceStateHandler(SequencerStateManager *manager,
                                           Sequencer *sequencer)
    : manager(manager), sequencer(sequencer)
{
}

SequenceStateHandler::~SequenceStateHandler() {}

void SequenceStateHandler::applyMessage(
    SequencerState &state, std::vector<utils::SimpleAction> &actions,
    const SequenceMessage &msg) const
{
    const auto visitor = Overload{
        [&](const TrackMessage &m)
        {
            manager->trackStateHandler->applyMessage(state, m);
        },
        [&](const SyncTrackEventIndices &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            const auto positionTicks = Sequencer::quarterNotesToTicks(
                state.transport.positionQuarterNotes);

            auto &tracks = state.sequences[m.sequenceIndex].tracks;

            for (auto &track : tracks)
            {
                if (positionTicks == 0 || !track.eventsHead)
                {
                    track.playEventIndex = EventIndex(0);
                    continue;
                }

                const EventData *ev = track.eventsHead;
                auto eventCounter = EventIndex(0);
                while (ev)
                {
                    if (ev->tick >= positionTicks)
                    {
                        track.playEventIndex = eventCounter;
                        break;
                    }
                    ++eventCounter;
                    ev = ev->next;
                }
            }

            lock.release();
        },
        [&](const SetLoopEnabled &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &seq = state.sequences[m.sequenceIndex];
            seq.loopEnabled = m.loopEnabled;

            lock.release();
        },
        [&](const SetSequenceUsed &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &seq = state.sequences[m.sequenceIndex];
            seq.used = m.used;

            lock.release();
        },
        [&](const SetTempoChangeEnabled &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &seq = state.sequences[m.sequenceIndex];
            seq.tempoChangeEnabled = m.tempoChangeEnabled;

            lock.release();
        },
        [&](const SetFirstLoopBarIndex &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &seq = state.sequences[m.sequenceIndex];
            seq.firstLoopBarIndex = m.barIndex;
            if (m.barIndex > seq.lastLoopBarIndex)
            {
                seq.lastLoopBarIndex = m.barIndex;
            }

            lock.release();
        },
        [&](const SetLastLoopBarIndex &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &seq = state.sequences[m.sequenceIndex];
            seq.lastLoopBarIndex = m.barIndex;

            if (m.barIndex != EndOfSequence &&
                m.barIndex < seq.firstLoopBarIndex)
            {
                seq.firstLoopBarIndex = m.barIndex;
            }

            lock.release();
        },
        [&](const SetInitialTempo &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            state.sequences[m.sequenceIndex].initialTempo = m.initialTempo;

            lock.release();
        },
        [&](const SetTimeSignature &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            const SequenceStateView stateView(state.sequences[m.sequenceIndex]);

            const auto barStart = stateView.getFirstTickOfBar(m.barIndex);

            const auto oldBarLength = stateView.getBarLength(m.barIndex);
            const auto newBarLength = m.timeSignature.getBarLength();
            const auto tickShift =
                static_cast<int>(newBarLength - oldBarLength);

            const auto cutStart = barStart + newBarLength;
            const auto cutEnd = barStart + oldBarLength;

            const auto nextBarStartTick =
                m.barIndex < Mpc2000XlSpecs::MAX_LAST_BAR_INDEX
                    ? stateView.getFirstTickOfBar(m.barIndex + 1)
                    : stateView.getLastTick();

            for (const auto &t : state.sequences[m.sequenceIndex].tracks)
            {
                EventData *it = t.eventsHead;
                while (it)
                {
                    const int tick = it->tick;

                    if (tick >= cutStart && tick < cutEnd)
                    {
                        manager->applyMessage(RemoveEvent{it});
                    }
                    else if (tick >= nextBarStartTick)
                    {
                        manager->applyMessage(
                            UpdateEventTick{it, tick + tickShift});
                    }
                    it = it->next;
                }
            }

            state.sequences[m.sequenceIndex].timeSignatures[m.barIndex] =
                m.timeSignature;
            state.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                m.timeSignature.getBarLength();

            lock.release();
        },
        [&](const SetLastBarIndex &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            state.sequences[m.sequenceIndex].lastBarIndex = m.barIndex;

            lock.release();
        },
        [&](const MoveTrack &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &tracks = state.sequences[m.sequenceIndex].tracks;
            const auto source = m.source;
            const auto dest = m.destination;

            size_t begin = std::min(source, dest);
            size_t end   = std::max(source, dest);

            if (source < dest)
            {
                std::rotate(tracks.begin() + source,
                            tracks.begin() + source + 1,
                            tracks.begin() + dest + 1);
            }
            else if (dest < source)
            {
                std::rotate(tracks.begin() + dest, tracks.begin() + source,
                            tracks.begin() + source + 1);
            }

            for (size_t i = begin; i <= end; i++)
                tracks[i].version++;

            lock.release();
        },
        [&](const InsertBars &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            applyInsertBars(m, state);

            lock.release();
        },
        [&](const DeleteBars &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            auto &seq = state.sequences[m.sequenceIndex];
            auto &ts = seq.timeSignatures;
            auto &bl = seq.barLengths;

            const int first = m.firstBarIndex;
            const int last = m.lastBarIndex;
            const int count = last - first + 1;

            const int total = seq.lastBarIndex + 1;

            const int tailBegin = last + 1;
            const int tailEnd = total;

            if (tailBegin < tailEnd)
            {
                std::rotate(ts.begin() + first, ts.begin() + tailBegin,
                            ts.begin() + tailEnd);

                std::rotate(bl.begin() + first, bl.begin() + tailBegin,
                            bl.begin() + tailEnd);
            }

            applyMessage(
                state, actions,
                SetLastBarIndex{m.sequenceIndex, seq.lastBarIndex - count});

            if (seq.lastBarIndex == NoBarIndex)
            {
                applyMessage(state, actions,
                             SetSequenceUsed{m.sequenceIndex, false});
            }

            lock.release();
        },
        [&](const DeleteTrack &m)
        {
            auto &lock = manager->trackLocks[m.sequenceIndex][m.trackIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            applyDeleteTrack(m, state);

            lock.release();
        },
        [&](const DeleteAllTracks &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
            {
                applyDeleteTrack(DeleteTrack{m.sequenceIndex, TrackIndex(i),
                                             m.deviceIndex, m.programChange,
                                             m.busType, m.velocityRatio},
                                 state);
            }

            lock.release();
        },
        [&](const UpdateSequenceEvents &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            applyUpdateSequenceEvents(m, state);

            lock.release();
        },
        [&](const UpdateSequenceTracks &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
            {
                state.sequences[m.sequenceIndex].tracks[i] =
                    (*m.trackStates)[i];
            }

            lock.release();
        },
        [&](const SetSequenceName &m)
        {
            auto &lock = manager->sequenceLocks[m.sequenceIndex];
            if (!lock.try_acquire())
            {
                manager->enqueue(m);
                return;
            }

            state.sequences[m.sequenceIndex].name.assign(m.name);

            lock.release();
        }};

    std::visit(visitor, msg);
}

void SequenceStateHandler::applyUpdateSequenceEvents(
    const UpdateSequenceEvents &m, SequencerState &state) const
{
    for (auto &[trackIndex, events] : *m.trackSnapshots)
    {
        assert(trackIndex <= Mpc2000XlSpecs::LAST_TRACK_INDEX);
        assert(trackIndex != TempoChangeTrackIndex);

        auto &track = state.sequences[m.sequenceIndex].tracks[trackIndex];

        for (const auto &src : events)
        {
            EventData *e = manager->acquireEvent();
            std::memcpy(e, &src, sizeof(EventData));
            e->sequenceIndex = m.sequenceIndex;
            e->trackIndex = TrackIndex(trackIndex);
            e->prev = nullptr;
            e->next = nullptr;
            manager->insertAcquiredEvent(track, e);
        }
    }
}

void SequenceStateHandler::applyDeleteTrack(const DeleteTrack &m,
                                            SequencerState &state) const
{
    manager->trackStateHandler->applyRemoveEvents(
        RemoveEvents{m.sequenceIndex, m.trackIndex}, state);

    auto &track = state.sequences[m.sequenceIndex].tracks[m.trackIndex];

    track.initializeDefaults();

    track.deviceIndex = m.deviceIndex;
    track.velocityRatio = m.velocityRatio;
    track.busType = m.busType;
    track.programChange = m.programChange;
}
