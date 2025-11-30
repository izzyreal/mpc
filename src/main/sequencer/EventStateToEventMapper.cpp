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
    class SequencerStateManager;
    std::shared_ptr<Event> mapEventStateToEvent(
        EventState *eventState,
        const std::function<void(SequenceMessage &&)> &dispatch,
        Sequence *parent)
    {
        if (eventState->type == EventType::NoteOn)
        {
            return std::make_shared<NoteOnEvent>(eventState, dispatch);
        }
        if (eventState->type == EventType::ChannelPressure)
        {
            return std::make_shared<ChannelPressureEvent>(eventState,
                                                          dispatch);
        }
        if (eventState->type == EventType::ControlChange)
        {
            return std::make_shared<ControlChangeEvent>(eventState,
                                                        dispatch);
        }
        if (eventState->type == EventType::Mixer)
        {
            return std::make_shared<MixerEvent>(eventState, dispatch);
        }
        if (eventState->type == EventType::PitchBend)
        {
            return std::make_shared<PitchBendEvent>(eventState, dispatch);
        }
        if (eventState->type == EventType::PolyPressure)
        {
            return std::make_shared<PolyPressureEvent>(eventState,
                                                       dispatch);
        }
        if (eventState->type == EventType::SystemExclusive)
        {
            return std::make_shared<SystemExclusiveEvent>(eventState,
                                                          dispatch);
        }
        if (eventState->type == EventType::ProgramChange)
        {
            return std::make_shared<ProgramChangeEvent>(eventState,
                                                        dispatch);
        }
        if (eventState->type == EventType::TempoChange)
        {
            return std::make_shared<TempoChangeEvent>(eventState,
                                                      dispatch, parent);
        }
        return {};
    }
} // namespace mpc::sequencer