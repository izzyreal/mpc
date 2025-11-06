#pragma once

#include <vector>

#include "mpc_types.hpp"

namespace mpc::lcdgui
{

    class Bressenham
    {
    public:
        static LcdLine Line(int x1, int y1, int x2, int y2);
    };
} // namespace mpc::lcdgui
