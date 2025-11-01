#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class TrackName : public TextualMetaEvent
    {

    public:
        void setName(std::string name);
        std::string getTrackName();

    public:
        TrackName(int tick, int delta, std::string name);
    };
} // namespace mpc::file::mid::event::meta
