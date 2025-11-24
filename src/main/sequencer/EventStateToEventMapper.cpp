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
#include "NonRtSequencerStateManager.hpp"

namespace mpc::sequencer
{
    class NonRtSequencerStateManager;
    std::shared_ptr<Event> mapEventStateToEvent(
        std::function<std::shared_ptr<NonRtTrackStateView>()> getTrackSnapshot,
        const EventState &e,
        const std::function<void(NonRtSequencerMessage &&)> &dispatch,
        Sequence *parent)
    {
        auto getEventSnapshot = [eventId = e.eventId, getTrackSnapshot]
        {
            return getTrackSnapshot()->getEventById(eventId);
        };

        if (e.type == EventType::NoteOn)
        {
            return std::make_shared<NoteOnEvent>(getEventSnapshot, dispatch);
        }
        if (e.type == EventType::ChannelPressure)
        {
            return std::make_shared<ChannelPressureEvent>(getEventSnapshot,
                                                          dispatch);
        }
        if (e.type == EventType::ControlChange)
        {
            return std::make_shared<ControlChangeEvent>(getEventSnapshot, dispatch);
        }
        if (e.type == EventType::Mixer)
        {
            return std::make_shared<MixerEvent>(getEventSnapshot, dispatch);
        }
        if (e.type == EventType::PitchBend)
        {
            return std::make_shared<PitchBendEvent>(getEventSnapshot, dispatch);
        }
        if (e.type == EventType::PolyPressure)
        {
            return std::make_shared<PolyPressureEvent>(getEventSnapshot, dispatch);
        }
        if (e.type == EventType::SystemExclusive)
        {
            return std::make_shared<SystemExclusiveEvent>(getEventSnapshot,
                                                          dispatch);
        }
        if (e.type == EventType::ProgramChange)
        {
            return std::make_shared<ProgramChangeEvent>(getEventSnapshot, dispatch);
        }
        if (e.type == EventType::TempoChange)
        {
            return std::make_shared<TempoChangeEvent>(getEventSnapshot, dispatch,
                                                      parent);
        }
        return {};
    }
} // namespace mpc::sequencer