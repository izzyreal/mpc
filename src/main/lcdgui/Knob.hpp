#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{
    class Knob : public Component
    {

        int value{32};
        bool color{true};

    public:
        void setValue(int newValue);
        void setColor(bool on);

        void Draw(std::vector<std::vector<bool>> *pixels) override;

        Knob(MRECT rect);
    };
} // namespace mpc::lcdgui
