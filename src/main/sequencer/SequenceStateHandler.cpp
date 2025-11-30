#include "sequencer/SequenceStateHandler.hpp"

#include "Sequence.hpp"
#include "SequenceStateView.hpp"
#include "Sequencer.hpp"
#include "SequencerStateManager.hpp"
#include "TrackStateHandler.hpp"
#include "sequencer/SequencerState.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

SequenceStateHandler::SequenceStateHandler(SequencerStateManager *manager,
                                           Sequencer *sequencer)
    : manager(manager), sequencer(sequencer)
{
}

SequenceStateHandler::~SequenceStateHandler() {}

void SequenceStateHandler::applyMessage(
    SequencerState &state, std::vector<std::function<void()>> &actions,
    const SequenceMessage &msg)
{
    const auto visitor = Overload{
        [&](const TrackMessage &m)
        {
            manager->trackStateHandler->applyMessage(state, actions, m);
        },
        [&](const SyncTrackEventIndices &)
        {
            sequencer->getSelectedSequence()->syncTrackEventIndices(
                Sequencer::quarterNotesToTicks(
                    state.transport.positionQuarterNotes));
        },
        [&](const SetLoopEnabled &m)
        {
            auto &seq = state.sequences[m.sequenceIndex];
            seq.loopEnabled = m.loopEnabled;
        },
        [&](const SetSequenceUsed &m)
        {
            auto &seq = state.sequences[m.sequenceIndex];
            seq.used = m.used;
        },
        [&](const SetTempoChangeEnabled &m)
        {
            auto &seq = state.sequences[m.sequenceIndex];
            seq.tempoChangeEnabled = m.tempoChangeEnabled;
        },
        [&](const SetFirstLoopBarIndex &m)
        {
            auto &seq = state.sequences[m.sequenceIndex];
            seq.firstLoopBarIndex = m.barIndex;
            if (m.barIndex > seq.lastLoopBarIndex)
            {
                seq.lastLoopBarIndex = m.barIndex;
            }
        },
        [&](const SetLastLoopBarIndex &m)
        {
            auto &seq = state.sequences[m.sequenceIndex];
            seq.lastLoopBarIndex = m.barIndex;

            if (m.barIndex != EndOfSequence &&
                m.barIndex < seq.firstLoopBarIndex)
            {
                seq.firstLoopBarIndex = m.barIndex;
            }
        },
        [&](const SetInitialTempo &m)
        {
            state.sequences[m.sequenceIndex].initialTempo = m.initialTempo;
        },
        [&](const UpdateBarLength &m)
        {
            state.sequences[m.sequenceIndex].barLengths[m.barIndex] = m.length;
        },
        [&](const UpdateBarLengths &m)
        {
            state.sequences[m.sequenceIndex].barLengths = m.barLengths;
        },
        [&](const UpdateTimeSignatures &m)
        {
            auto &seq = state.sequences[m.sequenceIndex];

            seq.timeSignatures = m.timeSignatures;

            for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
            {
                seq.barLengths[i] = seq.timeSignatures[i].getBarLength();
            }
        },
        [&](const UpdateTimeSignature &m)
        {
            state.sequences[m.sequenceIndex].timeSignatures[m.barIndex] =
                m.timeSignature;
            state.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                m.timeSignature.getBarLength();
        },
        [&](const SetLastBarIndex &m)
        {
            state.sequences[m.sequenceIndex].lastBarIndex = m.barIndex;
        },
        [&](const InsertBars &m)
        {
            applyInsertBars(m, state, actions);
        }};

    std::visit(visitor, msg);
}

void SequenceStateHandler::applyInsertBars(
    const InsertBars &m, SequencerState &state,
    std::vector<std::function<void()>> &actions) noexcept
{
    const auto seq = state.sequences[m.sequenceIndex];
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

    applyMessage(state, actions,
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

    applyMessage(state, actions, UpdateTimeSignatures{m.sequenceIndex, newTs});

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

        // TODO Tempo change track is excluded. We should double-check
        // if that is the desired behaviour.
        for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
        {
            auto it = seq.tracks[i].head;
            while (it)
            {
                if (it->tick >= barStart)
                {
                    const Tick newTick = it->tick + (newBarStart - barStart);
                    manager->trackStateHandler->applyMessage(state, actions, UpdateEventTick{it, newTick});
                }
                it = it->next;
            }
        }
    }

    actions.push_back(
        [a = m.onComplete, newLastBarIndex]
        {
            a(newLastBarIndex);
        });
}
