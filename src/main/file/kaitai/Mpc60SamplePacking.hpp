#pragma once

#include <cstdint>

namespace mpc::file::kaitai
{
    inline uint16_t canonicalMpc60SampleCode(const uint16_t packedCode,
                                             const bool secondInPair)
    {
        if (secondInPair)
        {
            return static_cast<uint16_t>(packedCode & 0x0fffU);
        }

        return static_cast<uint16_t>(((packedCode << 4U) | (packedCode >> 8U)) &
                                     0x0fffU);
    }
} // namespace mpc::file::kaitai
