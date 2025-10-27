#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{
    class EventRowParameters : public Component
    {

    public:
        void Draw(std::vector<std::vector<bool>> *pixels) override;

        EventRowParameters(MRECT rect);

        void setColor(bool on);

    private:
        bool color = false;
    };
} // namespace mpc::lcdgui
