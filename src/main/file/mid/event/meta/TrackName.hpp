#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class TrackName : public TextualMetaEvent
    {

    public:
        void setName(const std::string &name);
        std::string getTrackName();

        TrackName(int tick, int delta, const std::string &name);
    };
} // namespace mpc::file::mid::event::meta
