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
#include "TrackEventStateManager.hpp"

namespace mpc::sequencer
{
    class TrackEventStateManager;
    std::shared_ptr<Event> mapEventStateToEvent(
        std::shared_ptr<TrackEventStateManager> stateManager,
        const EventState &e,
        const std::function<void(TrackEventMessage &&)> &dispatch,
        Sequence *parent)
    {
        auto getSnapshot = [eventId = e.eventId, stateManager]
        {
            return stateManager->getSnapshot().getEventById(eventId);
        };

        if (e.type == EventType::NoteOn)
        {
            return std::make_shared<NoteOnEvent>(getSnapshot, dispatch);
        }
        if (e.type == EventType::ChannelPressure)
        {
            return std::make_shared<ChannelPressureEvent>(getSnapshot,
                                                          dispatch);
        }
        if (e.type == EventType::ControlChange)
        {
            return std::make_shared<ControlChangeEvent>(getSnapshot, dispatch);
        }
        if (e.type == EventType::Mixer)
        {
            return std::make_shared<MixerEvent>(getSnapshot, dispatch);
        }
        if (e.type == EventType::PitchBend)
        {
            return std::make_shared<PitchBendEvent>(getSnapshot, dispatch);
        }
        if (e.type == EventType::PolyPressure)
        {
            return std::make_shared<PolyPressureEvent>(getSnapshot, dispatch);
        }
        if (e.type == EventType::SystemExclusive)
        {
            return std::make_shared<SystemExclusiveEvent>(getSnapshot,
                                                          dispatch);
        }
        if (e.type == EventType::ProgramChange)
        {
            return std::make_shared<ProgramChangeEvent>(getSnapshot, dispatch);
        }
        if (e.type == EventType::TempoChange)
        {
            return std::make_shared<TempoChangeEvent>(getSnapshot, dispatch,
                                                      parent);
        }
        return {};
    }
} // namespace mpc::sequencer