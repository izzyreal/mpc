#include "sequencer/SequencerStateManager.hpp"

#include "sequencer/SequenceStateView.hpp"

using namespace mpc::sequencer;

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
        EventData *cur = destTrack.eventsHead;

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
                    destTrack.eventsHead = next;
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
    const EventData *src = sourceSeq.tracks[m.sourceTrackIndex].eventsHead;

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

