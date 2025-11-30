#include "sequencer/SequenceStateHandler.hpp"

#include "Sequence.hpp"
#include "SequenceStateView.hpp"
#include "Sequencer.hpp"
#include "SequencerStateManager.hpp"
#include "sequencer/SequencerState.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

SequenceStateHandler::SequenceStateHandler(SequencerStateManager *manager,
                                           Sequencer *sequencer)
    : manager(manager), sequencer(sequencer)
{
}

SequenceStateHandler::~SequenceStateHandler() {}

void updateEvent(EventState* e, const EventState& replacement)
{
    // Preserve links
    auto* prev = e->prev;
    auto* next = e->next;

    // Copy everything else (POD-style)
    *e = replacement;

    // Restore links
    e->prev = prev;
    e->next = next;
}

void SequenceStateHandler::applyMessage(
    SequencerState &state, std::vector<std::function<void()>> &actions,
    const SequenceMessage &msg)
{
    const auto visitor = Overload{
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
        [&](const SetUsed &m)
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
        [&](const UpdateTrackIndexOfAllEvents &m)
        {
            EventState *it = state.sequences[m.sequence].tracks[m.trackIndex].head;
            while (it)
            {
                it->trackIndex = m.trackIndex;
                it = it->next;
            }
        },
        [&](const FinalizeNonLiveNoteEvent &m)
        {
            EventState *it = state.sequences[m.noteOnEvent->sequenceIndex]
                               .tracks[m.noteOnEvent->trackIndex]
                               .head;
            while (it)
            {
                if (it->beingRecorded && it->duration == NoDuration &&
                    it->noteNumber == m.noteOnEvent->noteNumber)
                {
                    it->duration = m.duration;
                    it->beingRecorded = false;
                    break;
                }
                it = it->next;
            }
        },
        [&](const SetInitialTempo &m)
        {
            state.sequences[m.sequenceIndex].initialTempo = m.initialTempo;
        },
        [&](const UpdateEvent &m)
        {
            updateEvent(m.event, m.payload);
        },
        [&](const InsertEvent &m)
        {
            auto &track = state.sequences[m.eventState->sequenceIndex]
                               .tracks[m.eventState->trackIndex];

            manager->insertEvent(track, m.eventState, m.allowMultipleNoteEventsWithSameNoteOnSameTick);

            actions.push_back(m.onComplete);
        },
        [&](const ClearEvents &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];

            EventState* cur = track.head;
            while (cur)
            {
                EventState* nxt = cur->next;
                cur->prev = nullptr;
                cur->next = nullptr;
                manager->returnEventToPool(cur);
                cur = nxt;
            }
            track.head = nullptr;
        },
        [&](const RemoveDoubles &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];

            std::vector<NoteNumber> notesAtTick;
            Tick lastTick = -100;

            EventState* cur = track.head;

            while (cur)
            {
                EventState* next = cur->next;

                if (cur->type == EventType::NoteOn)
                {
                    if (cur->tick != lastTick)
                        notesAtTick.clear();

                    bool exists = false;
                    for (auto n : notesAtTick)
                        if (n == cur->noteNumber)
                            exists = true;

                    if (!exists)
                    {
                        notesAtTick.push_back(cur->noteNumber);
                        lastTick = cur->tick;
                    }
                    else
                    {
                        manager->freeEvent(track.head, cur);
                    }
                }

                cur = next;
            }
        },
        [&](const UpdateEventTick &m)
        {
            auto &track = state.sequences[m.eventState->sequenceIndex]
                              .tracks[m.eventState->trackIndex];

            EventState* e = m.eventState;
            const Tick newTick = m.newTick;
            const Tick oldTick = e->tick;

            if (newTick == oldTick)
                return;

            EventState*& head = track.head;

            if (e->prev) e->prev->next = e->next;
            else head = e->next;
            if (e->next) e->next->prev = e->prev;

            e->prev = nullptr;
            e->next = nullptr;
            e->tick = newTick;

            if (!head) { head = e; return; }

            EventState* cur = head;
            EventState* prev = nullptr;

            while (cur && cur->tick <= newTick)
            {
                prev = cur;
                cur = cur->next;
            }

            if (!prev)
            {
                e->next = head;
                head->prev = e;
                head = e;
                return;
            }

            e->prev = prev;
            e->next = cur;
            prev->next = e;
            if (cur) cur->prev = e;
        },
        [&](const RemoveEvent &m)
        {
            auto &track = state.sequences[m.sequence].tracks[m.track];
            EventState* e = m.eventState;
            if (!e) return;

            if (e->prev) e->prev->next = e->next;
            if (e->next) e->next->prev = e->prev;
            if (track.head == e) track.head = e->next;

            e->prev = nullptr;
            e->next = nullptr;

            manager->returnEventToPool(e);
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
        },
        [&](const UpdateEvents& m)
        {
            auto& track = state.sequences[m.sequence].tracks[m.track];

            EventState* cur = track.head;
            while (cur) {
                EventState* nxt = cur->next;
                cur->prev = nullptr;
                cur->next = nullptr;
                manager->freeEvent(track.head, cur);
                cur = nxt;
            }
            track.head = nullptr;

            for (const auto& src : m.snapshot) {
                EventState* e = manager->acquireEvent();
                std::memcpy(e, &src, sizeof(EventState));
                e->prev = nullptr;
                e->next = nullptr;
                manager->insertEvent(track, e, true);
            }
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
                    applyMessage(state, actions, UpdateEventTick{it, newTick});
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
