#pragma once
#include "file/mid/event/meta/TextualMetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class Text : public TextualMetaEvent
    {

    public:
        Text(int tick, int delta, const std::string &text);
    };
} // namespace mpc::file::mid::event::meta
