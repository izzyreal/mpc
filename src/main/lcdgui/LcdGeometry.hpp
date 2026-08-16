#pragma once

namespace mpc::lcdgui
{
    inline constexpr int LCD_WIDTH = 248;
    inline constexpr int LCD_HEIGHT = 60;

    struct LcdPoint
    {
        float x = 0.f;
        float y = 0.f;
    };
} // namespace mpc::lcdgui
