#include "sequencer/TrackEventStateManager.hpp"

using namespace mpc::sequencer;

TrackEventStateManager::TrackEventStateManager()
    : AtomicStateExchange([](TrackEventState &) {})
{
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
                for (auto &e : activeState.events)
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
                activeState.events[m.payload.first] = m.payload.second;
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
                activeState.events.clear();
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
                auto &events = activeState.events;

                const int oldIndex = m.eventIndex;
                const Tick newTick = m.newTick;

                // Extract the event
                EventState ev = events[oldIndex];
                events.erase(events.begin() + oldIndex);

                // Find the first element with tick > newTick
                auto it =
                    std::lower_bound(events.begin(), events.end(), newTick,
                                     [](const EventState &e, Tick t)
                                     {
                                         return e.tick < t;
                                     });

                // Insert at the right place
                events.insert(it, ev);

                // Update the tick after insertion
                events[it - events.begin()].tick = newTick;
            }
            else if constexpr (std::is_same_v<T, RemoveEvent>)
            {
                assert(m.eventIndex < activeState.events.size());
                activeState.events.erase(activeState.events.begin() +
                                         m.eventIndex);
            }
        },
        msg);
}
