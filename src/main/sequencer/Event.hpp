#pragma once
#include "IntTypes.hpp"
#include "NonRtSequencerMessage.hpp"
#include "sequencer/EventState.hpp"

#include <functional>

namespace mpc::sequencer
{
    class Event
    {
    public:
        explicit Event(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(NonRtSequencerMessage &&)> &dispatch);
        Event(const Event &);

        virtual ~Event() = default;

        void setTick(int tick) const;
        int getTick() const;
        void setTrack(TrackIndex) const;
        TrackIndex getTrack() const;

        virtual std::string getTypeName() const = 0;

        std::function<EventState()> getSnapshot;
        std::function<void(NonRtSequencerMessage &&)> dispatch;
    };
} // namespace mpc::sequencer
