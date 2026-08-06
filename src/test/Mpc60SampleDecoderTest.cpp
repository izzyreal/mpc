#include <catch2/catch_test_macros.hpp>

#include "TestMpc60SampleImport.hpp"
#include "file/kaitai/Mpc60SampleDecoder.hpp"
#include "file/kaitai/Mpc60SampleImport.hpp"
#include "file/kaitai/Mpc60SamplePacking.hpp"

#include <array>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <string_view>

using namespace mpc::file::kaitai;

TEST_CASE("MPC60 decoder matches pinned private-oracle sequence",
          "[mpc60-codec]")
{
    struct Step
    {
        uint16_t code;
        int16_t pcm;
    };

    constexpr std::array<Step, 20> expected{{
        {0x000, 0},     {0x001, 6},     {0x7ff, 13897}, {0x800, -4361},
        {0xfff, -3654}, {0x400, 4692},  {0xc00, -3574}, {0x123, -709},
        {0xabc, -9450}, {0x555, 2798},  {0xaaa, -6906}, {0x000, -4901},
        {0xfff, -3034}, {0x7ff, 12017}, {0x800, -5522}, {0x100, -2627},
        {0xf00, -3240}, {0x001, -2113}, {0x000, -1305}, {0x000, -806},
    }};

    Mpc60SampleDecoder decoder;
    for (const auto &step : expected)
    {
        REQUIRE(decoder.decodePcm(step.code) == step.pcm);
    }
}

TEST_CASE("MPC60 decoder reset restores initial state", "[mpc60-codec]")
{
    constexpr std::array<uint16_t, 8> codes{0x000, 0x7ff, 0x800, 0xfff,
                                            0x123, 0xabc, 0x555, 0xaaa};

    Mpc60SampleDecoder decoder;
    std::array<int16_t, codes.size()> first{};
    for (size_t i = 0; i < codes.size(); ++i)
    {
        first[i] = decoder.decodePcm(codes[i]);
    }

    decoder.reset();
    for (size_t i = 0; i < codes.size(); ++i)
    {
        REQUIRE(decoder.decodePcm(codes[i]) == first[i]);
    }
}

TEST_CASE("MPC60 packed positions normalize to one canonical code layout",
          "[mpc60-codec]")
{
    REQUIRE(canonicalMpc60SampleCode(0x0abc, false) == 0x0bca);
    REQUIRE(canonicalMpc60SampleCode(0x0def, true) == 0x0def);
    REQUIRE(canonicalMpc60SampleCode(0xffff, false) == 0x0fff);
    REQUIRE(canonicalMpc60SampleCode(0xffff, true) == 0x0fff);
}

TEST_CASE("MPC60 import policy uses environment and runtime override",
          "[mpc60-codec][mpc60-kill-switch]")
{
    mpc::test::ScopedMpc60ImportSetting importSetting(std::nullopt);
    const auto *environment =
        std::getenv("VMPC2000XL_DISABLE_MPC60_SAMPLE_IMPORT");
    const bool expectedEnvironmentDefault =
        environment == nullptr || std::string_view(environment) != "1";
    REQUIRE(Mpc60SampleImportPolicy::isEnabled() == expectedEnvironmentDefault);

    importSetting.set(false);
    REQUIRE_FALSE(Mpc60SampleImportPolicy::isEnabled());
    importSetting.set(true);
    REQUIRE(Mpc60SampleImportPolicy::isEnabled());
}
