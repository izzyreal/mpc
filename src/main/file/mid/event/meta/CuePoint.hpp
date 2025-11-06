#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class CuePoint : public TextualMetaEvent
    {

    public:
        void setCue(const std::string &name);
        std::string getCue();

        CuePoint(int tick, int delta, const std::string &marker);
    };
} // namespace mpc::file::mid::event::meta
