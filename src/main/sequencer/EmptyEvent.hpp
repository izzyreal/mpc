#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class EmptyEvent final : public Event
    {
    public:
        EmptyEvent();
        std::string getTypeName() const override
        {
            return "empty";
        }
    };
} // namespace mpc::sequencer
