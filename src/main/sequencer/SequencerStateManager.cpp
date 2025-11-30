#include "sequencer/SequencerStateManager.hpp"

#include "SequenceStateHandler.hpp"
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
}

SequencerStateManager::~SequencerStateManager() {}

void SequencerStateManager::returnEventToPool(EventState* e) const
{
    // Reset object to default "clean" state before returning it.
    e->resetToDefaultValues();

    // Return pointer to freelist
    pool->release(e);
}

void SequencerStateManager::freeEvent(EventState*& head, EventState* e) const
{
    // unlink from list
    if (e->prev)
        e->prev->next = e->next;
    else
        head = e->next;   // removing head

    if (e->next)
        e->next->prev = e->prev;

    // reset links
    e->prev = nullptr;
    e->next = nullptr;

    // return to freelist or push into an audio→main free queue
    // (replace with your actual mechanism)
    returnEventToPool(e);
}

EventState* SequencerStateManager::acquireEvent() const
{
    EventState* e = nullptr;
    if (!pool->acquire(e))
        return nullptr;

    new (e) EventState();  // placement-new calls resetToDefaultValues()
    return e;
}

void SequencerStateManager::applyMessage(
    const SequencerMessage &msg) noexcept
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
        [&](const CopyEvents &m)
        {
            applyCopyEvents(m);
        }};

    std::visit(visitor, msg);
}

void SequencerStateManager::applyCopyEvents(const CopyEvents &m) noexcept
{
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
        if (afterBar >= 998) break;

        applyMessage(InsertBars{m.destSequenceIndex, 1, afterBar});
        const TimeSignature ts{TimeSigNumerator(destNumerator),
                               TimeSigDenominator(destDenominator)};
        applyMessage(UpdateTimeSignature{m.destSequenceIndex, afterBar, ts});
    }

    if (!m.copyModeMerge)
    {
        EventState *prev = nullptr;
        EventState *cur = destTrack.head;

        while (cur)
        {
            const bool inRange =
                cur->tick >= destOffset &&
                cur->tick < destOffset + segLength * m.copyCount;

            EventState *next = cur->next;

            if (inRange)
            {
                if (prev) prev->next = next;
                else destTrack.head = next;
                if (next) next->prev = prev;
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
    const EventState *src = sourceSeq.tracks[m.sourceTrackIndex].head;

    while (src)
    {
        if (src->type == EventType::NoteOn &&
            !m.sourceNoteRange.contains(src->noteNumber))
        {
            src = src->next;
            continue;
        }

        if (src->tick >= m.sourceEndTick) break;

        if (src->tick >= m.sourceStartTick)
        {
            for (int copyIndex = 0; copyIndex < m.copyCount; copyIndex++)
            {
                const int tickCandidate =
                    src->tick + destOffset + copyIndex * segLength;

                if (tickCandidate >= destSeqView.getLastTick()) break;

                EventState *ev = acquireEvent();
                *ev = *src;

                ev->sequenceIndex = m.destSequenceIndex;
                ev->trackIndex = m.destTrackIndex;
                ev->tick = tickCandidate;

                insertEvent(destTrack, ev, true);
            }
        }

        src = src->next;
    }

    applyMessage(SetSelectedSequenceIndex{m.destSequenceIndex});
}

void SequencerStateManager::insertEvent(TrackState& track, EventState* e,
                 const bool allowMultipleNoteEventsWithSameNoteOnSameTick)
{
    assert(e);
    e->prev = nullptr;
    e->next = nullptr;

    EventState*& head = track.head;

    // === CASE 1: empty list ===
    if (!head) {
        head = e;
        return;
    }

    // === CASE 2: remove previous NoteOn at same tick? ===
    if (e->type == EventType::NoteOn &&
        !allowMultipleNoteEventsWithSameNoteOnSameTick)
    {
        const EventState * it = head;

        while (it) {
            if (it->type == EventType::NoteOn &&
                it->tick == e->tick &&
                it->noteNumber == e->noteNumber)
            {
                // unlink
                EventState* p = it->prev;
                EventState* n = it->next;
                if (p) p->next = n; else head = n;
                if (n) n->prev = p;

                // caller will later return it to the freelist
                break;
            }
            it = it->next;
        }
    }

    // === CASE 3: insert at head (smallest tick) ===
    if (e->tick < head->tick) {
        e->next = head;
        head->prev = e;
        head = e;
        return;
    }

    // === CASE 4: find insertion point ===
    EventState* it = head;
    while (it->next && it->next->tick <= e->tick) {
        it = it->next;
    }

    // insert after `it`
    EventState* n = it->next;

    it->next = e;
    e->prev = it;

    e->next = n;
    if (n) n->prev = e;
}
