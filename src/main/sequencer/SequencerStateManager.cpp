#include "sequencer/SequencerStateManager.hpp"

#include "SequenceStateHandler.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TransportStateHandler.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

SequencerStateManager::SequencerStateManager(Sequencer *sequencer)
    : AtomicStateExchange([](SequencerState &) {}), sequencer(sequencer)
{
    transportStateHandler =
        std::make_unique<TransportStateHandler>(this, sequencer);

    sequenceStateHandler =
        std::make_unique<SequenceStateHandler>(this, sequencer);
}

SequencerStateManager::~SequencerStateManager() {}

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

    auto destNumerator = -1;
    auto destDenominator = -1;
    auto destBarLength = -1;

    assert(m.sourceSequenceIndex >= 0 ||
           m.sourceSequenceIndex == SelectedSequenceIndex);

    const SequenceIndex sourceSequenceIndexToUse =
        m.sourceSequenceIndex >= MinSequenceIndex
            ? m.sourceSequenceIndex
            : activeState.selectedSequenceIndex;

    auto &destSeq = activeState.sequences[m.destSequenceIndex];
    const SequenceStateView destSeqView(destSeq);
    auto &destTrack = destSeq.tracks[m.destTrackIndex];

    const SequenceStateView destSequenceView(destSeq);

    const auto destSequenceBarCount = destSeq.lastBarIndex + 1;

    for (int i = 0; i < destSequenceBarCount; i++)
    {
        const auto firstTickOfBar =
            destSequenceView.getFirstTickOfBar(BarIndex(i));

        if (const auto barLength = destSequenceView.getBarLength(i);
            m.destStartTick >= firstTickOfBar &&
            m.destStartTick <= firstTickOfBar + barLength)
        {
            const auto ts = destSequenceView.getTimeSignature(i);
            destNumerator = ts.numerator;
            destDenominator = ts.denominator;
            destBarLength = barLength;
            break;
        }
    }

    const auto minimumRequiredNewSequenceLength = m.destStartTick + segLength;

    const auto ticksToAdd =
        minimumRequiredNewSequenceLength - destSequenceView.getLastTick();

    const auto barsToAdd =
        static_cast<int>(ceil(static_cast<float>(ticksToAdd) / destBarLength));

    const auto initialLastBarIndex = destSequenceView.getLastBarIndex();

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
        applyMessage(UpdateTimeSignature{m.destSequenceIndex, afterBar, ts});
    }

    if (!m.copyModeMerge)
    {
        auto &destTrackEvents = destTrack.events;

        for (auto it = destTrackEvents.begin(); it != destTrackEvents.end();)
        {
            if (it->tick >= destOffset &&
                it->tick < destOffset + segLength * m.copyCount)
            {
                destTrackEvents.erase(it);
                continue;
            }
            ++it;
        }
    }

    const auto &sourceSeq = activeState.sequences[sourceSequenceIndexToUse];
    const auto sourceTrackEvents = sourceSeq.tracks[m.sourceTrackIndex].events;

    for (auto &e : sourceTrackEvents)
    {
        if (e.type == EventType::NoteOn &&
            !m.sourceNoteRange.contains(e.noteNumber))
        {
            continue;
        }

        if (e.tick >= m.sourceEndTick)
        {
            break;
        }

        if (e.tick >= m.sourceStartTick)
        {
            for (int copyIndex = 0; copyIndex < m.copyCount; copyIndex++)
            {
                const int tickCandidate =
                    e.tick + destOffset + copyIndex * segLength;

                if (tickCandidate >= destSeqView.getLastTick())
                {
                    break;
                }

                EventState eventCopy = e;
                eventCopy.eventId = m.generateEventId();
                eventCopy.sequenceIndex = m.destSequenceIndex;
                eventCopy.trackIndex = m.destTrackIndex;
                eventCopy.tick = tickCandidate;
                applyMessage(InsertEvent{eventCopy});
            }
        }
    }

    applyMessage(SetSelectedSequenceIndex{m.destSequenceIndex});
}
