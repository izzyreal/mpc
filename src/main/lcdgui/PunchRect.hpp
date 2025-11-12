#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{

    class PunchRect : public Component
    {

        bool on = false;

    public:
        void setOn(bool newOn);

        void Draw(std::vector<std::vector<bool>> *pixels) override;

        PunchRect(const std::string &name, MRECT rect);
    };
} // namespace mpc::lcdgui
