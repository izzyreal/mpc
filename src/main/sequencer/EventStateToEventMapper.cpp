#include "sequencer/EventStateToEventMapper.hpp"

#include "sequencer/NoteOnEvent.hpp"

#include "EventType.hpp"

namespace mpc::sequencer
{
    std::shared_ptr<Event> mapEventStateToEvent(
        const EventState &e,
        const std::function<void(TrackEventMessage &&)> &dispatch)
    {
        if (e.type == EventType::NoteOn)
        {
            return std::make_shared<NoteOnEvent>(
                [e]
                {
                    return e;
                },
                dispatch);
        }
        return {};
    }
} // namespace mpc::sequencer