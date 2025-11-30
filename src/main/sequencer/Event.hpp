#pragma once
#include "IntTypes.hpp"
#include "SequenceMessage.hpp"
#include "sequencer/EventState.hpp"

#include <functional>

namespace mpc::sequencer
{
    class Event
    {
    public:
        explicit Event(
            EventState *eventState,
            const std::function<void(SequenceMessage &&)> &dispatch);
        Event(const Event &);

        virtual ~Event() = default;

        void setTick(int tick) const;
        int getTick() const;
        void setTrack(TrackIndex) const;
        TrackIndex getTrack() const;

        virtual std::string getTypeName() const = 0;

        EventState *eventState = nullptr;
        std::function<void(SequenceMessage &&)> dispatch;
    };
} // namespace mpc::sequencer
