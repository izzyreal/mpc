#include "sequencer/EventStateToEventMapper.hpp"

#include "ControlChangeEvent.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/ChannelPressureEvent.hpp"

#include "EventType.hpp"
#include "MixerEvent.hpp"
#include "PitchBendEvent.hpp"
#include "PolyPressureEvent.hpp"
#include "ProgramChangeEvent.hpp"
#include "SystemExclusiveEvent.hpp"
#include "TempoChangeEvent.hpp"

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
        else if (e.type == EventType::ChannelPressure)
        {
            return std::make_shared<ChannelPressureEvent>(
                [e]
                {
                    return e;
                },
                dispatch);
        }
        else if (e.type == EventType::ControlChange)
        {
            return std::make_shared<ControlChangeEvent>(
                [e]
                {
                    return e;
                },
                dispatch);
        }
        else if (e.type == EventType::Mixer)
        {
            return std::make_shared<MixerEvent>([e]{return e;}, dispatch);
        }
        else if (e.type == EventType::PitchBend)
        {
                    return std::make_shared<PitchBendEvent>([e]{return e;}, dispatch);
        }
        else if (e.type == EventType::PolyPressure)
        {
            return std::make_shared<PolyPressureEvent>([e]{return e;}, dispatch);
        }
        else if (e.type == EventType::SystemExclusive)
        {
            return std::make_shared<SystemExclusiveEvent>([e]{return e;}, dispatch);
        }
        else if (e.type == EventType::ProgramChange)
        {
            return std::make_shared<ProgramChangeEvent>([e]{return e;}, dispatch);
        }
        else if (e.type == EventType::TempoChange)
        {
            // return std::make_shared<TempoChangeEvent>([e]{return e;}, dispatch);
        }
        return {};
    }
} // namespace mpc::sequencer