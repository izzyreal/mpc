#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{

    class Rectangle : public Component
    {

        bool on = false;

    public:
        void setOn(bool newOn);

        void Draw(std::vector<std::vector<bool>> *pixels) override;

        Rectangle(const std::string &name, MRECT rect);
    };
} // namespace mpc::lcdgui
