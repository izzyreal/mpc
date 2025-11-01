#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class InstrumentName : public TextualMetaEvent
    {

        public:
            void setName(std::string name);
            std::string getName();

            InstrumentName(int tick, int delta, std::string name);
    };
}
