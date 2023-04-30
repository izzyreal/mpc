#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>

namespace mpc::sampleops {

    inline float clamp_mean_normalized_float(const float &f)
    {
        return std::clamp(f, -1.0f, 1.0f);
    }

    inline void clamp_mean_normalized_floats(std::vector<float> &floats)
    {
        for (auto &f: floats)
        {
            f = clamp_mean_normalized_float(f);
        }
    }

    inline int16_t mean_normalized_float_to_short(const float &f)
    {
        float scaled = f * 32768.0f;
        float clamped = std::max<float>(-32768.0f, std::min<float>(scaled, 32767.0f));
        return static_cast<int16_t>(std::roundf(clamped));
    }

    inline float short_to_float(const int16_t s)
    {
        return static_cast<float>(s) / 32768;
    }
}