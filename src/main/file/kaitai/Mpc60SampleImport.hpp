#pragma once

#include <cstdint>
#include <vector>

namespace mpc::file::kaitai
{
    inline constexpr bool kMpc60SampleImportEnabled = false;

    inline constexpr const char *kMpc60SndLoadingDisabledMessage =
        "MPC60 SND loading disabled";
    inline constexpr const char *kMpc60SetLoadingDisabledMessage =
        "MPC60 SET loading disabled";

    inline bool isMpc60SndBytes(const std::vector<char> &bytes)
    {
        return bytes.size() >= 2 &&
               static_cast<uint8_t>(bytes[0]) == 0x01 &&
               static_cast<uint8_t>(bytes[1]) == 0x01;
    }

    inline bool isMpc60SetBytes(const std::vector<char> &bytes)
    {
        return bytes.size() >= 2 &&
               static_cast<uint8_t>(bytes[0]) == 0x02 &&
               (static_cast<uint8_t>(bytes[1]) == 0x00 ||
                static_cast<uint8_t>(bytes[1]) == 0x01);
    }
}
