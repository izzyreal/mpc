#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class EmptyEvent final : public EventRef
    {
    public:
        EmptyEvent();
        std::string getTypeName() const override
        {
            return "empty";
        }
    };
} // namespace mpc::sequencer
