#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class InstrumentName : public TextualMetaEvent
    {

    public:
        void setName(const std::string &name);
        std::string getName();

        InstrumentName(int tick, int delta, const std::string &name);
    };
} // namespace mpc::file::mid::event::meta
