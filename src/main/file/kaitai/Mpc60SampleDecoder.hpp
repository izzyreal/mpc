#pragma once

#include <cstdint>

namespace mpc::file::kaitai
{
    constexpr int kMpc60SampleRate = 40000;

    class Mpc60SampleDecoder
    {
    public:
        int16_t decodeImportedPcm(uint16_t sampleWord, bool isOddWordIndex);
        float decodeImportedFloat(uint16_t sampleWord, bool isOddWordIndex);
    };
}
