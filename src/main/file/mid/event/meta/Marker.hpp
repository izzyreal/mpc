#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class Marker : public TextualMetaEvent
    {

    public:
        void setMarkerName(std::string name);
        std::string getMarkerName();

        Marker(int tick, int delta, std::string marker);
    };
} // namespace mpc::file::mid::event::meta
