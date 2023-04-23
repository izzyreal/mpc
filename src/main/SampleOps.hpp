#pragma once

#include <cstdint>
#include <vector>

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
        auto rounded = static_cast<int16_t>(std::round(scaled));
        return std::max(static_cast<int16_t>(-32768), std::min(rounded, static_cast<int16_t>(32767)));
    }
}