#pragma once

#include <cstdint>

namespace mpc::file::kaitai
{
    constexpr int kMpc60SampleRate = 40000;

    class Mpc60SampleDecoder
    {
    public:
        int16_t decodePcm(uint16_t canonicalCode);
        float decodeFloat(uint16_t canonicalCode);
        void reset();

    private:
        int32_t state = 0;
    };
} // namespace mpc::file::kaitai
