#include "Sequence.hpp"
#include "Sequencer.hpp"
#include "Track.hpp"
#include "TrackStateView.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include "sequencer/SequenceStateView.hpp"

using namespace mpc::sequencer;

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

    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; i++)
    {
        const auto t1 = fromSequence.getTrack(i);

        if (!t1.isUsed())
        {
            continue;
        }

        auto t1Events =
            t1.getEventRange(firstTickOfFromSequence, lastTickOfFromSequence);

        sequencer->getSequence(m.toSeqIndex)
            ->getTrack(i)
            ->setUsedIfCurrentlyUnused();

        const auto toSequenceLastTick = toSequence.getLastTick();

        for (const auto &event : t1Events)
        {
            const auto firstCopyTick = event->tick + offset;

            if (firstCopyTick >= toSequenceLastTick)
            {
                // The events in Track are ordered by tick, so this and
                // any following event copies would be out of bounds.
                break;
            }

            for (auto k = 0; k < m.copyCount; k++)
            {
                const auto tick = firstCopyTick + k * segmentLengthTicks;

                // We do a more specific exit-check here, since within-bounds
                // copies may be followed by out-of-bounds ones.
                if (tick >= toSequenceLastTick)
                {
                    break;
                }

                const auto eventCopy = acquireEvent();
                *eventCopy = *event;
                eventCopy->sequenceIndex = SequenceIndex(m.toSeqIndex);
                eventCopy->tick = tick;
                applyMessage(InsertAcquiredEvent{eventCopy});
            }
        }
    }
}
