#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{

    class Rectangle : public Component
    {

    private:
        bool on = false;

    public:
        void setOn(bool newOn);

    public:
        void Draw(std::vector<std::vector<bool>> *pixels) override;

    public:
        Rectangle(const std::string &name, MRECT rect);
    };
} // namespace mpc::lcdgui
