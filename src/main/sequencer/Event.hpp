#pragma once
#include "IntTypes.hpp"
#include "performance/Sequence.hpp"

#include <functional>

namespace mpc::sequencer
{
    class Event
    {
    public:
        explicit Event(const std::function<performance::Event()> &getSnapshot);
        Event(const Event &);

        virtual ~Event() = default;
        bool dontDelete = false;
        int wasMoved = 0;

        void setTick(int relativeTick);
        int getTick() const;
        virtual void setTrack(TrackIndex);
        TrackIndex getTrack() const;

        virtual std::string getTypeName() const = 0;

        std::function<performance::Event()> getSnapshot;

    protected:
        int tick = 0;
        TrackIndex track{0};
    };
} // namespace mpc::sequencer
