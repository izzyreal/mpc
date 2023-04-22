#include "SampleOps.hpp"

#include <cassert>
#include <algorithm>

namespace mpc::sampleops {

    void assert_mean_normalized_float(const float &f)
    {
        assert(f >= -1.0f && f <= 1.0f);
    }

    void assert_shift_normalized_float(const float &f)
    {
        assert(f >= 0.0f && f <= 2.0f);
    }

    float clamp_mean_normalized_float(const float &f)
    {
        return std::clamp(f, -1.0f, 1.0f);
    }

    void clamp_mean_normalized_floats(std::vector<float> &floats)
    {
        for (auto &f: floats)
        {
            f = clamp_mean_normalized_float(f);
        }
    }

    float clamp_shift_normalized_float(const float &f)
    {
        assert_shift_normalized_float(f);
        return std::clamp(f, 0.0f, 2.0f);
    }

    float mean_normalize_float(const float &f)
    {
        assert_shift_normalized_float(f);
        return f - 1.0f;
    }

    float shift_normalize_float(const float &f)
    {
        assert_mean_normalized_float(f);
        return f + 1.0f;
    }

    uint16_t shift_normalized_float_to_ushort(const float &f)
    {
        assert_shift_normalized_float(f);
        return static_cast<uint16_t>(f / mpc::sampleops::inv_32768);
    }

    float ushort_to_shift_normalized_float(const uint16_t &i)
    {
        auto res = static_cast<float>(i) * mpc::sampleops::inv_32768;
        assert_shift_normalized_float(res);
        return res;
    }

    uint16_t short_to_ushort(const int16_t &i)
    {
        return static_cast<uint16_t>(i & 0xFFFF);
    }

    int16_t ushort_to_short(const uint16_t &i)
    {
        auto res = static_cast<int16_t>(i);
        if (res & 0x8000)
        { res |= static_cast<int16_t>(0xFFFF << 16); }
        return res;
    }

}