#include "sequencer/EventDataToEventMapper.hpp"

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
    std::shared_ptr<EventRef>
    mapEventDataToEvent(EventData *const ptr, const EventData &snapshot,
                        const std::function<void(TrackMessage &&)> &dispatch,
                        Sequence *parent)
    {
        if (snapshot.type == EventType::NoteOn)
        {
            return std::make_shared<NoteOnEvent>(ptr, snapshot, dispatch);
        }
        if (snapshot.type == EventType::ChannelPressure)
        {
            return std::make_shared<ChannelPressureEvent>(ptr, snapshot,
                                                          dispatch);
        }
        if (snapshot.type == EventType::ControlChange)
        {
            return std::make_shared<ControlChangeEvent>(ptr, snapshot,
                                                        dispatch);
        }
        if (snapshot.type == EventType::Mixer)
        {
            return std::make_shared<MixerEvent>(ptr, snapshot, dispatch);
        }
        if (snapshot.type == EventType::PitchBend)
        {
            return std::make_shared<PitchBendEvent>(ptr, snapshot, dispatch);
        }
        if (snapshot.type == EventType::PolyPressure)
        {
            return std::make_shared<PolyPressureEvent>(ptr, snapshot, dispatch);
        }
        if (snapshot.type == EventType::SystemExclusive)
        {
            return std::make_shared<SystemExclusiveEvent>(ptr, snapshot,
                                                          dispatch);
        }
        if (snapshot.type == EventType::ProgramChange)
        {
            return std::make_shared<ProgramChangeEvent>(ptr, snapshot,
                                                        dispatch);
        }
        if (snapshot.type == EventType::TempoChange)
        {
            return std::make_shared<TempoChangeEvent>(ptr, snapshot, dispatch,
                                                      parent);
        }
        return {};
    }
} // namespace mpc::sequencer