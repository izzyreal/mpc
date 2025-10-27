#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

#include <string>

namespace mpc::midi::event::meta
{

    class TrackName : public TextualMetaEvent
    {

    public:
        void setName(std::string name);
        std::string getTrackName();

    public:
        TrackName(int tick, int delta, std::string name);
    };
} // namespace mpc::midi::event::meta
