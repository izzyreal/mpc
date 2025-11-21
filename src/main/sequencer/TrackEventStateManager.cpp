#include "sequencer/TrackEventStateManager.hpp"

using namespace mpc::sequencer;

TrackEventStateManager::TrackEventStateManager()
    : AtomicStateExchange([](TrackEventState &) {})
{
}

void moveEvent(std::vector<EventState> &v, const size_t oldIndex,
               const size_t newIndex)
{
    if (oldIndex > newIndex)
    {
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex,
                    v.rend() - newIndex);
    }
    else
    {
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1,
                    v.begin() + newIndex + 1);
    }
}

void TrackEventStateManager::applyMessage(const TrackEventMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, UpdateTrackIndexOfAllEvents>)
            {
                for (auto &e : activeState.events)
                {
                    e.trackIndex = m.trackIndex;
                }
            }
            else if constexpr (std::is_same_v<T, FinalizeNonLiveNoteEvent>)
            {
                activeState.events[m.noteOnEvent.eventIndex].duration = m.duration;
                activeState.events[m.noteOnEvent.eventIndex].beingRecorded = false;
            }
            else if constexpr (std::is_same_v<T, UpdateEvent>)
            {
                activeState.events[m.eventState.eventIndex] = m.eventState;
            }
            else if constexpr (std::is_same_v<T, InsertEvent>)
            {
                auto &events = activeState.events;

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
                        events.back().eventIndex = EventIndex(events.size() - 1);
                    }
                    else
                    {
                        auto insertedEvent = events.emplace(insertAt, m.eventState);
                        insertedEvent->eventIndex = EventIndex(insertAt - events.begin());
                    }
                }
                else
                {
                    events.emplace_back(m.eventState);
                    events.back().eventIndex = EventIndex(events.size() - 1);
                }

                activeState.eventIndex = activeState.eventIndex + 1;
            }
            else if constexpr (std::is_same_v<T, ClearEvents>)
            {
                activeState.events.clear();
                activeState.eventIndex = EventIndex(0);
            }
            else if constexpr (std::is_same_v<T, AddToEventIndex>)
            {
                activeState.eventIndex = activeState.eventIndex + m.valueToAdd;
            }
            else if constexpr (std::is_same_v<T, SyncEventIndex>)
            {
                if (m.currentTick == 0)
                {
                    activeState.eventIndex = EventIndex(0);
                    return;
                }

                int startIndex = 0;

                if (m.currentTick > m.previousTick)
                {
                    if (activeState.eventIndex == activeState.events.size())
                    {
                        return;
                    }

                    startIndex = activeState.eventIndex;
                }

                if (m.currentTick < m.previousTick &&
                    activeState.eventIndex == 0)
                {
                    return;
                }

                auto result{EventIndex(activeState.events.size())};

                for (int i = startIndex; i < activeState.events.size(); i++)
                {
                    if (activeState.events[i].tick >= m.currentTick)
                    {
                        result = EventIndex(i);
                        break;
                    }
                }

                activeState.eventIndex = result;
            }
            else if constexpr (std::is_same_v<T, RemoveDoubles>)
            {
                auto eventCounter = 0;
                std::vector<int> deleteIndexList;
                std::vector<int> notesAtTick;
                int lastTick = -100;

                auto &events = activeState.events;

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
                int currentIndex = -1;
                int newIndex = -1;

                bool higherOneExists = false;

                auto &events = activeState.events;

                for (int i = 0; i < events.size(); i++)
                {
                    if (currentIndex >= 0 && newIndex >= 0)
                    {
                        break;
                    }

                    if (events[i] == m.eventState)
                    {
                        currentIndex = i;
                        continue;
                    }

                    if (newIndex == -1 && events[i].tick > m.newTick)
                    {
                        higherOneExists = true;
                        newIndex = i - 1;
                    }
                }

                if (!higherOneExists)
                {
                    newIndex = events.size() - 1;
                }

                moveEvent(events, currentIndex, newIndex);

                events[newIndex].tick = m.newTick;
            }
            else if constexpr (std::is_same_v<T, RemoveEvent>)
            {
                for (auto it = activeState.events.begin();
                     it != activeState.events.end(); ++it)
                {
                    if (*it == m.eventState)
                    {
                        activeState.events.erase(it);
                        break;
                    }
                }
            }
            else if constexpr (std::is_same_v<T, RemoveEventByIndex>)
            {
                activeState.events.erase(activeState.events.begin() +
                                         m.eventIndex);
            }
            else if constexpr (std::is_same_v<T, RemoveDontDeleteFlag>)
            {
                activeState.events[m.eventIndex].dontDelete = false;
            }
        },
        msg);
}
