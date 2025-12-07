#include "sequencer/SequenceStateHandler.hpp"

#include "sequencer/SequenceStateView.hpp"
#include "sequencer/SequencerState.hpp"
#include "SequencerStateManager.hpp"

using namespace mpc::sequencer;

void SequenceStateHandler::applyInsertBars(
    const InsertBars &m, const SequencerState &state,
    std::vector<utils::SimpleAction> &actions) const noexcept
{
    const auto &seq = state.sequences[m.sequenceIndex];
    const SequenceStateView seqView(seq);

    const auto oldLastBarIndex = seqView.getLastBarIndex();
    const bool isAppending = m.afterBar - 1 == oldLastBarIndex;

    auto barCountToUse = m.barCount;

    if (oldLastBarIndex + barCountToUse > Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
    {
        barCountToUse = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX - oldLastBarIndex;
    }

    if (barCountToUse == 0)
    {
        return;
    }

    const auto &oldTs = seqView.getTimeSignatures();

    const auto newLastBarIndex = oldLastBarIndex + barCountToUse;

    manager->applyMessage(
        SetLastBarIndex{m.sequenceIndex, BarIndex(newLastBarIndex)});

    std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> newTs{};

    int out = 0;

    for (int i = 0; i <= oldLastBarIndex; ++i)
    {
        if (i == m.afterBar)
        {
            for (int j = 0;
                 j < barCountToUse && out < Mpc2000XlSpecs::MAX_BAR_COUNT; ++j)
            {
                newTs[out++] =
                    TimeSignature{TimeSigNumerator(4), TimeSigDenominator(4)};
            }
        }
        if (out < Mpc2000XlSpecs::MAX_BAR_COUNT)
        {
            newTs[out++] = oldTs[i];
        }
    }

    for (; out < Mpc2000XlSpecs::MAX_BAR_COUNT; ++out)
    {
        newTs[out] = TimeSignature{};
    }

    manager->applyMessage(UpdateTimeSignatures{m.sequenceIndex, newTs});

    int barStart = 0;

    for (int i = 0; i < m.afterBar; ++i)
    {
        barStart += seqView.getBarLength(i);
    }

    if (!isAppending)
    {
        int newBarStart = 0;
        for (int i = 0; i < m.afterBar + barCountToUse; ++i)
        {
            newBarStart += seqView.getBarLength(i);
        }

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
        [a = m.onComplete, newLastBarIndex]() mutable
        {
            a(newLastBarIndex);
        });

    actions.emplace_back(m.nextAction);
}
