#include "sequencer/SequenceStateHandler.hpp"

#include "sequencer/SequenceStateView.hpp"
#include "sequencer/SequencerState.hpp"
#include "SequencerStateManager.hpp"

using namespace mpc::sequencer;

void SequenceStateHandler::applyInsertBars(
    const InsertBars &m, SequencerState &state,
    std::vector<utils::SimpleAction> &actions) const noexcept
{
    auto &seq = state.sequences[m.sequenceIndex];
    const SequenceStateView seqView(seq);

    const auto oldLastBarIndex = seqView.getLastBarIndex();
    const bool isAppending = m.afterBar - 1 == oldLastBarIndex;

    auto barCountToUse = m.barCount;
    if (oldLastBarIndex + barCountToUse > Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
        barCountToUse = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX - oldLastBarIndex;

    if (barCountToUse == 0)
        return;

    const auto newLastBarIndex = oldLastBarIndex + barCountToUse;

    manager->applyMessage(
        SetLastBarIndex{m.sequenceIndex, BarIndex(newLastBarIndex)});

    auto &ts = seq.timeSignatures;
    auto &bl = seq.barLengths;

    const int insertPos = m.afterBar;
    const int shiftCount = barCountToUse;
    const int activeCount = oldLastBarIndex + 1;

    if (activeCount > insertPos)
    {
        const int end = insertPos + shiftCount + (activeCount - insertPos);
        const int realEnd = std::min<int>(end, Mpc2000XlSpecs::MAX_BAR_COUNT);

        std::rotate(ts.begin() + insertPos,
                    ts.begin() + activeCount,
                    ts.begin() + realEnd);

        std::rotate(bl.begin() + insertPos,
                    bl.begin() + activeCount,
                    bl.begin() + realEnd);
    }

    for (int i = 0; i < shiftCount; ++i)
    {
        ts[insertPos + i] = TimeSignature{TimeSigNumerator(4), TimeSigDenominator(4)};
        bl[insertPos + i] = ts[insertPos + i].getBarLength();
    }

    int barStart = 0;
    for (int i = 0; i < m.afterBar; ++i)
        barStart += seqView.getBarLength(i);

    if (!isAppending)
    {
        int newBarStart = 0;
        for (int i = 0; i < m.afterBar + barCountToUse; ++i)
            newBarStart += seqView.getBarLength(i);

        for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; ++i)
        {
            auto it = seq.tracks[i].eventsHead;
            while (it)
            {
                if (it->tick >= barStart)
                {
                    const Tick newTick = it->tick + (newBarStart - barStart);
                    manager->enqueue(UpdateEventTick{it, newTick});
                }
                it = it->next;
            }
        }
    }

    actions.emplace_back(
        [a = m.onComplete, newLastBarIndex]
        {
            a(newLastBarIndex);
        });

    actions.emplace_back(m.nextAction);
}
