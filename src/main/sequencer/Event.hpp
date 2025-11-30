#pragma once
#include "IntTypes.hpp"
#include "SequencerMessage.hpp"
#include "sequencer/EventState.hpp"

#include <functional>

namespace mpc::sequencer
{
    class Event
    {
    public:
        explicit Event(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(SequencerMessage &&)> &dispatch);
        Event(const Event &);

        virtual ~Event() = default;

        void setTick(int tick) const;
        int getTick() const;
        void setTrack(TrackIndex) const;
        TrackIndex getTrack() const;

        virtual std::string getTypeName() const = 0;

        std::function<EventState()> getSnapshot;
        std::function<void(SequencerMessage &&)> dispatch;
    };
} // namespace mpc::sequencer
