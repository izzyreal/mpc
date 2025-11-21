#pragma once
#include "IntTypes.hpp"
#include "TrackEventMessage.hpp"
#include "sequencer/EventState.hpp"

#include <functional>

namespace mpc::sequencer
{
    class Event
    {
    public:
        explicit Event(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);
        Event(const Event &);

        virtual ~Event() = default;
        bool dontDelete = false;
        int wasMoved = 0;

        void setTick(int relativeTick);
        int getTick() const;
        virtual void setTrack(TrackIndex);
        TrackIndex getTrack() const;

        virtual std::string getTypeName() const = 0;

        std::function<EventState()> getSnapshot;
        std::function<void(TrackEventMessage &&)> dispatch;

    protected:
        TrackIndex track{0};
    };
} // namespace mpc::sequencer
