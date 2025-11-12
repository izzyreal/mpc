#pragma once

#include <cstdint>
#include <chrono>

namespace mpc::utils
{
    inline int64_t nowInNanoseconds() noexcept
    {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(
                   steady_clock::now().time_since_epoch())
            .count();
    }

    inline int64_t nowInMilliseconds() noexcept
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
                   steady_clock::now().time_since_epoch())
            .count();
    }
} // namespace mpc::utils