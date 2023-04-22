#pragma once

#include <cstdint>
#include <vector>

namespace mpc::sampleops {

    static const float inv_32768 = 1.0f / 32768;

    inline void assert_mean_normalized_float(const float& f);

    inline void assert_shift_normalized_float(const float& f);

    inline float clamp_mean_normalized_float(const float& f);

    void clamp_mean_normalized_floats(std::vector<float>& floats);

    inline float clamp_shift_normalized_float(const float& f);

    inline float mean_normalize_float(const float& f);

    inline float shift_normalize_float(const float& f);

    inline uint16_t shift_normalized_float_to_ushort(const float& f);

    inline float ushort_to_shift_normalized_float(const uint16_t& i);

    inline uint16_t short_to_ushort(const int16_t& i);

    inline int16_t ushort_to_short(const uint16_t& i);
}