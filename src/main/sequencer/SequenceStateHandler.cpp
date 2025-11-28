#include "sequencer/SequenceStateHandler.hpp"

#include "Sequence.hpp"
#include "SequenceStateView.hpp"
#include "Sequencer.hpp"
#include "SequencerStateManager.hpp"
#include "Transport.hpp"
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
    const SequenceMessage &msg, const bool autoRefreshPlaybackState)
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;

            if constexpr (std::is_same_v<T, SetLoopEnabled>)
            {
                auto &seq = state.sequences[m.sequenceIndex];
                seq.loopEnabled = m.loopEnabled;
            }
            else if constexpr (std::is_same_v<T, SetUsed>)
            {
                auto &seq = state.sequences[m.sequenceIndex];
                seq.used = m.used;
            }
            else if constexpr (std::is_same_v<T, SetTempoChangeEnabled>)
            {
                auto &seq = state.sequences[m.sequenceIndex];
                seq.tempoChangeEnabled = m.tempoChangeEnabled;
            }
            else if constexpr (std::is_same_v<T, SetFirstLoopBarIndex>)
            {
                auto &seq = state.sequences[m.sequenceIndex];
                seq.firstLoopBarIndex = m.barIndex;
                if (m.barIndex > seq.lastLoopBarIndex)
                {
                    seq.lastLoopBarIndex = m.barIndex;
                }
            }
            else if constexpr (std::is_same_v<T, SetLastLoopBarIndex>)
            {
                auto &seq = state.sequences[m.sequenceIndex];
                seq.lastLoopBarIndex = m.barIndex;

                if (m.barIndex != EndOfSequence &&
                    m.barIndex < seq.firstLoopBarIndex)
                {
                    seq.firstLoopBarIndex = m.barIndex;
                }
            }
            else if constexpr (std::is_same_v<T, UpdateTrackIndexOfAllEvents>)
            {
                // printf("Applying UpdateTrackIndexOfAllEvents\n");
                for (auto &e :
                     state.sequences[m.sequence].tracks[m.trackIndex].events)
                {
                    e.trackIndex = m.trackIndex;
                }
            }
            else if constexpr (std::is_same_v<T, FinalizeNonLiveNoteEvent>)
            {
                // printf("Applying FinalizeNonLiveNoteEvent\n");
                for (auto &e : state.sequences[m.noteOnEvent.sequenceIndex]
                                   .tracks[m.noteOnEvent.trackIndex]
                                   .events)
                {
                    if (e.beingRecorded && e.duration == NoDuration &&
                        e.noteNumber == m.noteOnEvent.noteNumber)
                    {
                        e.duration = m.duration;
                        e.beingRecorded = false;
                        break;
                    }
                }
            }
            else if constexpr (std::is_same_v<T, SetInitialTempo>)
            {
                state.sequences[m.sequenceIndex].initialTempo = m.initialTempo;
            }
            else if constexpr (std::is_same_v<T, UpdateEvent>)
            {
                // printf("Applying UpdateEvent\n");
                for (auto &e : state.sequences[m.payload.sequenceIndex]
                                   .tracks[m.payload.trackIndex]
                                   .events)
                {
                    if (e.eventId == m.payload.eventId)
                    {
                        e = m.payload;
                    }
                }
            }
            else if constexpr (std::is_same_v<T, InsertEvent>)
            {
                // printf("Applying InsertEvent\n");
                assert(m.eventState.eventId != NoEventId);
                auto &events = state.sequences[m.eventState.sequenceIndex]
                                   .tracks[m.eventState.trackIndex]
                                   .events;

                if (m.eventState.type == EventType::NoteOn &&
                    !m.allowMultipleNoteEventsWithSameNoteOnSameTick)
                {
                    for (auto it = events.begin(); it != events.end(); ++it)
                    {
                        if (it->type == EventType::NoteOn)
                        {
                            if (it->tick == m.eventState.tick &&
                                it->noteNumber == m.eventState.noteNumber)
                            {
                                events.erase(it);
                                break;
                            }
                        }
                    }
                }

                const bool insertRequired =
                    !events.empty() && events.back().tick >= m.eventState.tick;

                if (insertRequired)
                {
                    const auto insertAt = std::find_if(
                        events.begin(), events.end(),
                        [tick = m.eventState.tick](const EventState &e)
                        {
                            return e.tick > tick;
                        });

                    if (insertAt == events.end())
                    {
                        events.emplace_back(m.eventState);
                    }
                    else
                    {
                        events.emplace(insertAt, m.eventState);
                    }
                }
                else
                {
                    events.emplace_back(m.eventState);
                }

                // printf("Inserted event in sequence %i track %i tick %llu\n",
                //     m.eventState.sequenceIndex.get(),
                //     m.eventState.trackIndex.get(), m.eventState.tick);

                actions.push_back(m.onComplete);
            }
            else if constexpr (std::is_same_v<T, ClearEvents>)
            {
                // printf("Applying ClearEvents\n");
                state.sequences[m.sequence].tracks[m.track].events.clear();
            }
            else if constexpr (std::is_same_v<T, RemoveDoubles>)
            {
                // printf("Applying RemoveDoubles\n");
                auto eventCounter = 0;
                std::vector<int> deleteIndexList;
                std::vector<int> notesAtTick;
                int lastTick = -100;

                auto &events =
                    state.sequences[m.sequence].tracks[m.track].events;

                for (auto &e : events)
                {
                    if (e.type == EventType::NoteOn)
                    {
                        if (lastTick != e.tick)
                        {
                            notesAtTick.clear();
                        }

                        bool contains = false;

                        for (const auto &n : notesAtTick)
                        {
                            if (n == e.noteNumber)
                            {
                                contains = true;
                                break;
                            }
                        }

                        if (!contains)
                        {
                            notesAtTick.push_back(e.noteNumber);
                        }
                        else
                        {
                            deleteIndexList.push_back(eventCounter);
                        }

                        lastTick = e.tick;
                    }
                    eventCounter++;
                }

                std::reverse(deleteIndexList.begin(), deleteIndexList.end());

                for (const auto &i : deleteIndexList)
                {
                    events.erase(events.begin() + i);
                }
            }
            else if constexpr (std::is_same_v<T, UpdateEventTick>)
            {
                // printf("Applying UpdateEventTick\n");
                auto &events = state.sequences[m.eventState.sequenceIndex]
                                   .tracks[m.eventState.trackIndex]
                                   .events;
                auto it = std::find_if(
                    events.begin(), events.end(),
                    [eventId = m.eventState.eventId](const EventState &e)
                    {
                        return e.eventId == eventId;
                    });
                assert(it != events.end());
                const auto oldIndex = it - events.begin();
                const Tick newTick = m.newTick;

                EventState ev = events[oldIndex];

                events.erase(events.begin() + oldIndex);

                auto it2 =
                    std::lower_bound(events.begin(), events.end(), newTick,
                                     [](const EventState &e, const Tick t)
                                     {
                                         return e.tick < t;
                                     });

                events.insert(it2, ev);
                const auto newIndex = it2 - events.begin();
                events[newIndex].tick = newTick;
            }
            else if constexpr (std::is_same_v<T, RemoveEvent>)
            {
                // printf("Applying RemoveEvent\n");
                auto &events =
                    state.sequences[m.sequence].tracks[m.track].events;
                auto it =
                    std::find_if(events.begin(), events.end(),
                                 [eventId = m.eventId](const EventState &e)
                                 {
                                     return e.eventId == eventId;
                                 });
                assert(it != events.end());
                events.erase(it);
            }
            else if constexpr (std::is_same_v<T, UpdateBarLength>)
            {
                // printf("Applying UpdateBarLength\n");
                state.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                    m.length;
            }
            else if constexpr (std::is_same_v<T, UpdateBarLengths>)
            {
                // printf("Applying UpdateBarLengths\n");
                state.sequences[m.sequenceIndex].barLengths = m.barLengths;
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignatures>)
            {
                // printf("Applying UpdateTimeSignatures\n");
                auto &seq = state.sequences[m.sequenceIndex];

                seq.timeSignatures = m.timeSignatures;

                for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
                {
                    seq.barLengths[i] = seq.timeSignatures[i].getBarLength();
                }
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignature>)
            {
                // printf("Applying UpdateTimeSignature\n");
                state.sequences[m.sequenceIndex].timeSignatures[m.barIndex] =
                    m.timeSignature;
                state.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                    m.timeSignature.getBarLength();
            }
            else if constexpr (std::is_same_v<T, SetLastBarIndex>)
            {
                state.sequences[m.sequenceIndex].lastBarIndex = m.barIndex;
            }
            else if constexpr (std::is_same_v<T, InsertBars>)
            {
                applyInsertBars(m, state, actions);
            }
            else if constexpr (std::is_same_v<T, UpdateEvents>)
            {
                // printf("Applying UpdateEvents\n");
                state.sequences[m.sequence].tracks[m.track].events =
                    m.eventStates;

                EventId eventId = MinEventId;
                for (auto &e :
                     state.sequences[m.sequence].tracks[m.track].events)
                {
                    e.sequenceIndex = m.sequence;
                    e.trackIndex = m.track;
                    e.eventId = eventId++;
                }
            }
            else if constexpr (std::is_same_v<T, UpdateSequenceEvents>)
            {
                printf("Applying UpdateSequenceEvents\n");

                auto &sequence = state.sequences[m.sequenceIndex];

                for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; ++i)
                {
                    sequence.tracks[i].events.clear();
                }

                EventId eventId = MinEventId;

                for (const auto &e : m.eventStates)
                {
                    auto &events = sequence.tracks[e.trackIndex].events;
                    events.push_back(e);
                    events.back().sequenceIndex = m.sequenceIndex;
                    events.back().eventId = eventId++;
                }
            }
        },
        msg);

    if (!autoRefreshPlaybackState)
    {
        return;
    }

    const auto isPlaying = sequencer->getTransport()->isPlaying();
    const auto playbackState =
        sequencer->getStateManager()->getSnapshot().getPlaybackState();

    if (isPlaying &&
        isVariantAnyOf(
            msg, SequenceMessagesThatInvalidatePlaybackStateWhilePlaying{}))
    {
        manager->publishState();
        manager->applyMessage(RefreshPlaybackStateWhilePlaying{});
    }
    else if (!isPlaying &&
             isVariantAnyOf(
                 msg,
                 SequenceMessagesThatInvalidatePlaybackStateWhileNotPlaying{}))
    {
        manager->publishState();
        manager->applyMessage(RefreshPlaybackStateWhileNotPlaying{});
    }
}

void SequenceStateHandler::applyInsertBars(
    const InsertBars &m, SequencerState &state,
    std::vector<std::function<void()>> &actions) noexcept
{
    constexpr bool autoRefreshPlaybackState = false;

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
                 SetLastBarIndex{m.sequenceIndex, BarIndex(newLastBarIndex)},
                 autoRefreshPlaybackState);

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

    applyMessage(state, actions, UpdateTimeSignatures{m.sequenceIndex, newTs},
                 autoRefreshPlaybackState);

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
            for (auto &e : seq.tracks[i].events)
            {
                if (e.tick >= barStart)
                {
                    const Tick newTick = e.tick + (newBarStart - barStart);
                    applyMessage(state, actions, UpdateEventTick{e, newTick},
                                 autoRefreshPlaybackState);
                }
            }
        }
    }

    actions.push_back(
        [a = m.onComplete, newLastBarIndex]
        {
            a(newLastBarIndex);
        });

    manager->applyMessage(RefreshPlaybackStateWhileNotPlaying{});
}
