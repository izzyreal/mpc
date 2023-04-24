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
        auto rounded = static_cast<int16_t>(std::roundf(scaled));
        return std::max<int16_t>(static_cast<int16_t>(-32768), std::min<int16_t>(rounded, static_cast<int16_t>(32767)));
    }

    inline float short_to_float(const int16_t s)
    {
        return static_cast<float>(s) / 32768;
    }
}