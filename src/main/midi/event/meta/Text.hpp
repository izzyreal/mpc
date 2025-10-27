#pragma once
#include <midi/event/meta/TextualMetaEvent.hpp>

namespace mpc::midi::event::meta
{

    class Text : public TextualMetaEvent
    {

    public:
        Text(int tick, int delta, std::string text);
    };
} // namespace mpc::midi::event::meta
