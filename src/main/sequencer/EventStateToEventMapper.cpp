#include "sequencer/EventStateToEventMapper.hpp"

#include "sequencer/EventType.hpp"

#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"

namespace mpc::sequencer
{
    class SequencerStateManager;
    std::shared_ptr<Event> mapEventStateToEvent(
        EventState *eventState,
        const std::function<void(TrackMessage &&)> &dispatch,
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