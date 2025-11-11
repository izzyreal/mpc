#pragma once
#include "Component.hpp"

#include <string>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    class Label;
    class Field;
} // namespace mpc::lcdgui

namespace mpc::lcdgui
{
    class Parameter : public Component
    {

    public:
        Parameter(Mpc &mpc, std::string labelStr, std::string name, int x,
                  int y, int fieldWidth);
    };
} // namespace mpc::lcdgui
