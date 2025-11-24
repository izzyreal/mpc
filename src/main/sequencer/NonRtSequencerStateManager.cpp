#include "sequencer/NonRtSequencerStateManager.hpp"

using namespace mpc::sequencer;

NonRtSequencerStateManager::NonRtSequencerStateManager()
    : AtomicStateExchange([](NonRtSequencerState &) {})
{
}

NonRtSequencerStateManager::~NonRtSequencerStateManager()
{
    //    printf("~NonRtSequencerStateManager\n");
}

void NonRtSequencerStateManager::applyMessage(const NonRtSequencerMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, UpdateTrackIndexOfAllEvents>)
            {
                for (auto &e : activeState.sequences[m.sequence].tracks[m.trackIndex].events)
                {
                    e.trackIndex = m.trackIndex;
                }
            }
            else if constexpr (std::is_same_v<T, FinalizeNonLiveNoteEvent>)
            {
                for (auto &e : activeState.sequences[m.noteOnEvent.sequenceIndex].tracks[m.noteOnEvent.trackIndex].events)
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
            else if constexpr (std::is_same_v<T, UpdateEvent>)
            {
                for (auto &e : activeState.sequences[m.payload.sequenceIndex].tracks[m.payload.trackIndex].events)
                {
                    if (e.eventId == m.payload.eventId)
                    {
                        e = m.payload;
                    }
                }
            }
            else if constexpr (std::is_same_v<T, InsertEvent>)
            {
                assert(m.eventState.eventId != NoEventId);
                auto &events = activeState.sequences[m.eventState.sequenceIndex].tracks[m.eventState.trackIndex].events;

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

                actions.push_back(m.onComplete);
            }
            else if constexpr (std::is_same_v<T, ClearEvents>)
            {
                activeState.sequences[m.sequence].tracks[m.track].events.clear();
            }
            else if constexpr (std::is_same_v<T, RemoveDoubles>)
            {
                auto eventCounter = 0;
                std::vector<int> deleteIndexList;
                std::vector<int> notesAtTick;
                int lastTick = -100;

                auto &events = activeState.sequences[m.sequence].tracks[m.track].events;

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
                auto &events = activeState.sequences[m.eventState.sequenceIndex].tracks[m.eventState.trackIndex].events;
                auto it =
                    std::find_if(events.begin(), events.end(),
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
                                     [](const EventState &e, Tick t)
                                     {
                                         return e.tick < t;
                                     });

                events.insert(it2, ev);
                const auto newIndex = it2 - events.begin();
                events[newIndex].tick = newTick;
            }
            else if constexpr (std::is_same_v<T, RemoveEvent>)
            {
                auto &events = activeState.sequences[m.sequence].tracks[m.track].events;
                auto it =
                    std::find_if(events.begin(), events.end(),
                                 [eventId = m.eventId](const EventState &e)
                                 {
                                     return e.eventId == eventId;
                                 });
                assert(it != events.end());
                events.erase(it);
            }
        },
        msg);
}
