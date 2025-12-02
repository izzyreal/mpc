#include "sequencer/SequencerStateManager.hpp"

#include "SequenceStateHandler.hpp"
#include "Track.hpp"
#include "TrackStateHandler.hpp"
#include "TrackStateView.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TransportStateHandler.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;
using namespace mpc::concurrency;

SequencerStateManager::SequencerStateManager(Sequencer *sequencer)
    : AtomicStateExchange([](SequencerState &) {}), sequencer(sequencer)
{
    pool = std::make_shared<EventStateFreeList>();

    transportStateHandler =
        std::make_unique<TransportStateHandler>(this, sequencer);

    sequenceStateHandler =
        std::make_unique<SequenceStateHandler>(this, sequencer);

    trackStateHandler = std::make_unique<TrackStateHandler>(this, sequencer);
}

SequencerStateManager::~SequencerStateManager() {}

void SequencerStateManager::returnEventToPool(EventData *e) const
{
    // Reset object to default "clean" state before returning it.
    e->resetToDefaultValues();

    // Return pointer to freelist
    pool->release(e);
}

EventData *SequencerStateManager::acquireEvent() const
{
    EventData *e = nullptr;
    if (!pool->acquire(e))
    {
        return nullptr;
    }

    new (e) EventData(); // placement-new calls resetToDefaultValues()
    return e;
}

void SequencerStateManager::applyMessage(const SequencerMessage &msg) noexcept
{
    auto visitor = Overload{
        [&](const TransportMessage &m)
        {
            transportStateHandler->applyMessage(activeState.transport, m);
        },
        [&](const SequenceMessage &m)
        {
            sequenceStateHandler->applyMessage(activeState, actions, m);
        },
        [&](const TrackMessage &m)
        {
            trackStateHandler->applyMessage(activeState, actions, m);
        },
        [&](const SwitchToNextSequence &m)
        {
            constexpr bool setPositionTo0 = false;
            enqueue(SetSelectedSequenceIndex{m.sequenceIndex, setPositionTo0});
        },
        [&](const SetSelectedSequenceIndex &m)
        {
            activeState.selectedSequenceIndex = m.sequenceIndex;

            if (m.setPositionTo0)
            {
                activeState.transport.positionQuarterNotes = 0;
            }
        },
        [&](const CopyBars &m)
        {
            applyCopyBars(m);
        },
        [&](const CopyEvents &m)
        {
            applyCopyEvents(m);
        }};

    std::visit(visitor, msg);
}

void SequencerStateManager::applyCopyEvents(const CopyEvents &m) noexcept
{
    auto &lock = trackLocks[m.destSequenceIndex][m.destTrackIndex];

    if (!lock.try_acquire())
    {
        enqueue(m);
        return;
    }

    const auto segLength = m.sourceEndTick - m.sourceStartTick;
    const auto destOffset = m.destStartTick - m.sourceStartTick;

    int destNumerator = -1;
    int destDenominator = -1;
    int destBarLength = -1;

    const SequenceIndex sourceSequenceIndexToUse =
        m.sourceSequenceIndex >= MinSequenceIndex
            ? m.sourceSequenceIndex
            : activeState.selectedSequenceIndex;

    auto &destSeq = activeState.sequences[m.destSequenceIndex];
    const SequenceStateView destSeqView(destSeq);
    auto &destTrack = destSeq.tracks[m.destTrackIndex];

    const auto destSequenceBarCount = destSeq.lastBarIndex + 1;

    for (int i = 0; i < destSequenceBarCount; i++)
    {
        const auto firstTickOfBar = destSeqView.getFirstTickOfBar(BarIndex(i));
        const auto barLength = destSeqView.getBarLength(i);

        if (m.destStartTick >= firstTickOfBar &&
            m.destStartTick <= firstTickOfBar + barLength)
        {
            const auto ts = destSeqView.getTimeSignature(i);
            destNumerator = ts.numerator;
            destDenominator = ts.denominator;
            destBarLength = barLength;
            break;
        }
    }

    const auto minimumRequiredNewSequenceLength = m.destStartTick + segLength;
    const auto ticksToAdd =
        minimumRequiredNewSequenceLength - destSeqView.getLastTick();

    const auto barsToAdd =
        static_cast<int>(ceil(static_cast<float>(ticksToAdd) / destBarLength));

    const auto initialLastBarIndex = destSeqView.getLastBarIndex();

    for (int i = 0; i < barsToAdd; i++)
    {
        const auto afterBar = initialLastBarIndex + i + 1;
        if (afterBar >= 998)
        {
            break;
        }

        applyMessage(InsertBars{m.destSequenceIndex, 1, afterBar});
        const TimeSignature ts{TimeSigNumerator(destNumerator),
                               TimeSigDenominator(destDenominator)};
        applyMessage(SetTimeSignature{m.destSequenceIndex, afterBar, ts});
    }

    if (!m.copyModeMerge)
    {
        EventData *prev = nullptr;
        EventData *cur = destTrack.head;

        while (cur)
        {
            const bool inRange =
                cur->tick >= destOffset &&
                cur->tick < destOffset + segLength * m.copyCount;

            EventData *next = cur->next;

            if (inRange)
            {
                if (prev)
                {
                    prev->next = next;
                }
                else
                {
                    destTrack.head = next;
                }
                if (next)
                {
                    next->prev = prev;
                }
                returnEventToPool(cur);
            }
            else
            {
                prev = cur;
            }

            cur = next;
        }
    }

    const auto &sourceSeq = activeState.sequences[sourceSequenceIndexToUse];
    const EventData *src = sourceSeq.tracks[m.sourceTrackIndex].head;

    while (src)
    {
        if (src->type == EventType::NoteOn &&
            !m.sourceNoteRange.contains(src->noteNumber))
        {
            src = src->next;
            continue;
        }

        if (src->tick >= m.sourceEndTick)
        {
            break;
        }

        if (src->tick >= m.sourceStartTick)
        {
            for (int copyIndex = 0; copyIndex < m.copyCount; copyIndex++)
            {
                const int tickCandidate =
                    src->tick + destOffset + copyIndex * segLength;

                if (tickCandidate >= destSeqView.getLastTick())
                {
                    break;
                }

                EventData *ev = acquireEvent();
                *ev = *src;

                ev->sequenceIndex = m.destSequenceIndex;
                ev->trackIndex = m.destTrackIndex;
                ev->tick = tickCandidate;

                insertAcquiredEvent(destTrack, ev);
            }
        }

        src = src->next;
    }

    lock.release();

    applyMessage(SetSelectedSequenceIndex{m.destSequenceIndex});
}

void SequencerStateManager::applyCopyBars(const CopyBars &m) noexcept
{
    int sourceBarCounter = 0;

    const auto numberOfSourceBars = m.copyLastBar + 1 - m.copyFirstBar;

    for (int i = 0; i < m.destinationBarCount; ++i)
    {
        const TimeSignature ts =
            activeState.sequences[m.fromSeqIndex]
                .timeSignatures[m.copyFirstBar + sourceBarCounter];

        applyMessage(
            SetTimeSignature{m.toSeqIndex, BarIndex(i + m.copyAfterBar), ts});

        sourceBarCounter++;

        if (sourceBarCounter >= numberOfSourceBars)
        {
            sourceBarCounter = 0;
        }
    }

    const SequenceStateView fromSequence(activeState.sequences[m.fromSeqIndex]);
    const SequenceStateView toSequence(activeState.sequences[m.toSeqIndex]);

    const auto firstTickOfFromSequence =
        fromSequence.getFirstTickOfBar(m.copyFirstBar);

    const auto lastTickOfFromSequence =
        fromSequence.getFirstTickOfBar(m.copyLastBar + 1);

    const auto firstTickOfToSequence =
        toSequence.getFirstTickOfBar(m.copyAfterBar);

    const auto segmentLengthTicks =
        lastTickOfFromSequence - firstTickOfFromSequence;

    const auto offset = firstTickOfToSequence - firstTickOfFromSequence;

    for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; i++)
    {
        const auto t1 = fromSequence.getTrack(i);

        if (!t1->isUsed())
        {
            continue;
        }

        auto t1Events =
            t1->getEventRange(firstTickOfFromSequence, lastTickOfFromSequence);

        const auto t2 = toSequence.getTrack(i);

        if (!t2->isUsed())
        {
            sequencer->getSequence(m.toSeqIndex)->getTrack(i)->setUsed(true);
        }

        for (const auto &event : t1Events)
        {
            const auto firstCopyTick = event->tick + offset;

            for (auto k = 0; k < m.copyCount; k++)
            {
                const auto tick = firstCopyTick + k * segmentLengthTicks;
                const auto eventCopy = acquireEvent();
                *eventCopy = *event;
                eventCopy->sequenceIndex = SequenceIndex(m.toSeqIndex);
                eventCopy->tick = tick;
                applyMessage(InsertAcquiredEvent{eventCopy});
            }
        }
    }
}

void SequencerStateManager::insertAcquiredEvent(TrackState &track, EventData *e)
{
    assert(e);

    track.playEventIndex = track.playEventIndex + 1;

    e->prev = nullptr;
    e->next = nullptr;

    EventData *&head = track.head;

    if (!head)
    {
        head = e;
        return;
    }

    if (e->tick < head->tick)
    {
        e->next = head;
        head->prev = e;
        head = e;
        return;
    }

    EventData *it = head;
    while (it->next && it->next->tick <= e->tick)
    {
        it = it->next;
    }

    EventData *n = it->next;

    it->next = e;
    e->prev = it;

    e->next = n;
    if (n)
    {
        n->prev = e;
    }
}
