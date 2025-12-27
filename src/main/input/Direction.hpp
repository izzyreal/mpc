#pragma once

namespace mpc::input
{
    enum class Direction
    {
        NoDirection,
        Positive,
        Negative
    };

    constexpr int toSign(const Direction d)
    {
        switch (d)
        {
            case Direction::Positive:
                return +1;
            case Direction::Negative:
                return -1;
            case Direction::NoDirection:
            default:
                return 0;
        }
    }
} // namespace mpc::input