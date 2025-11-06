#pragma once
#include "Observer.hpp"

namespace mpc::sequencer
{
    class Event : public Observable
    {
    public:
        bool dontDelete = false;
        int wasMoved = 0;

        void setTick(int relativeTick);
        int getTick() const;
        virtual void setTrack(int i);
        int getTrack() const;

        virtual std::string getTypeName() const = 0;
        Event() = default;
        Event(const Event &);

    protected:
        int tick = 0;
        int track = 0;
    };
} // namespace mpc::sequencer
