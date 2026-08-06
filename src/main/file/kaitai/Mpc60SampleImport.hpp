#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace mpc::file::kaitai
{
    class Mpc60SampleImportPolicy
    {
    public:
        static bool isEnabled();

        // Overrides the process environment for subsequent operations. Passing
        // std::nullopt restores the environment-controlled default.
        static void setRuntimeOverride(std::optional<bool> enabled);
    };

    inline constexpr const char *kMpc60SndLoadingDisabledMessage =
        "MPC60 SND loading disabled";
    inline constexpr const char *kMpc60SetLoadingDisabledMessage =
        "MPC60 SET loading disabled";

    inline bool isMpc60SndBytes(const std::vector<char> &bytes)
    {
        return bytes.size() >= 2 && static_cast<uint8_t>(bytes[0]) == 0x01 &&
               static_cast<uint8_t>(bytes[1]) == 0x01;
    }

    inline bool isMpc60SetBytes(const std::vector<char> &bytes)
    {
        return bytes.size() >= 2 && static_cast<uint8_t>(bytes[0]) == 0x02 &&
               (static_cast<uint8_t>(bytes[1]) == 0x00 ||
                static_cast<uint8_t>(bytes[1]) == 0x01);
    }
} // namespace mpc::file::kaitai
