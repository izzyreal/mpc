#include "sequencer/EventStateToEventMapper.hpp"

#include "sequencer/NoteOnEvent.hpp"

#include "EventState.hpp"
#include "EventType.hpp"

namespace mpc::sequencer
{
    std::shared_ptr<Event> mapEventStateToEvent(const EventState &e)
    {
        if (e.type == EventType::NoteOn)
        {
            return std::make_shared<NoteOnEvent>(
                [e]
                {
                    return e;
                });
        }
        return {};
    }
} // namespace mpc::sequencer