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
    const SequenceMessage &msg) const
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
        [&](const SetTimeSignature &m)
        {
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
                EventData *it = t.head;
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
        },
        [&](const SetLastBarIndex &m)
        {
            state.sequences[m.sequenceIndex].lastBarIndex = m.barIndex;
        },
        [&](const MoveTrack &m)
        {
            auto &tracks = state.sequences[m.sequenceIndex].tracks;
            const auto source = m.source;
            const auto dest = m.destination;

            if (source < dest) {
                std::rotate(tracks.begin() + source,
                            tracks.begin() + source + 1,
                            tracks.begin() + dest + 1);
            } else if (dest < source) {
                std::rotate(tracks.begin() + dest,
                            tracks.begin() + source,
                            tracks.begin() + source + 1);
            }
        },
        [&](const InsertBars &m)
        {
            applyInsertBars(m, state, actions);
        }};

    std::visit(visitor, msg);
}

void SequenceStateHandler::applyInsertBars(
    const InsertBars &m, const SequencerState &state,
    std::vector<std::function<void()>> &actions) const noexcept
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
            auto it = seq.tracks[i].head;

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

    actions.push_back(
        [a = m.onComplete, newLastBarIndex]
        {
            a(newLastBarIndex);
        });
}
