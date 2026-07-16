#include <catch2/catch_test_macros.hpp>

#include "file/kaitai/Mpc60SetPreview.hpp"

#include <cmrc/cmrc.hpp>

#include <string_view>
#include <vector>

CMRC_DECLARE(mpctest);

namespace
{
    std::vector<char> readResource(const std::string &path)
    {
        const auto fs = cmrc::mpctest::get_filesystem();
        const auto file = fs.open(path);
        return std::vector<char>(file.begin(), file.end());
    }
}

TEST_CASE("MPC60 SET preview maps MPC60 pads to directory entries",
          "[kaitai-set][real-mpc60]")
{
    using mpc::file::kaitai::Mpc60SetPreviewLoader;

    const auto rockPreview = Mpc60SetPreviewLoader::loadPreview(
        readResource("test/RealMpc60/SetV0/ROCK.SET"));
    const auto studioPreview = Mpc60SetPreviewLoader::loadPreview(
        readResource("test/RealMpc60/SetV0/STUDIO.SET"));
    const auto uk8Preview = Mpc60SetPreviewLoader::loadPreview(
        readResource("test/RealMpc60/SetV0/UK-8.SET"));
    const auto studioV1Preview = Mpc60SetPreviewLoader::loadPreview(
        readResource("test/RealMpc60/SetV1/STUDIO.SET"));

    REQUIRE(rockPreview.totalNumberOfSampleWords == 510936U);
    REQUIRE(studioPreview.totalNumberOfSampleWords == 519560U);
    REQUIRE(uk8Preview.totalNumberOfSampleWords == 517520U);
    REQUIRE(studioV1Preview.totalNumberOfSampleWords == 519560U);

    REQUIRE(rockPreview.useMasterMixData);
    REQUIRE(studioPreview.useMasterMixData);
    REQUIRE(uk8Preview.useMasterMixData);
    REQUIRE(studioV1Preview.useMasterMixData);

    REQUIRE(rockPreview.soundDirectoryEntries.size() == 34U);
    REQUIRE(studioPreview.soundDirectoryEntries.size() == 34U);
    REQUIRE(uk8Preview.soundDirectoryEntries.size() == 34U);
    REQUIRE(studioV1Preview.soundDirectoryEntries.size() == 34U);
    REQUIRE(rockPreview.soundDirectoryEntryIndexByMpc60Pad.size() == 34U);
    REQUIRE(studioPreview.soundDirectoryEntryIndexByMpc60Pad.size() == 34U);
    REQUIRE(uk8Preview.soundDirectoryEntryIndexByMpc60Pad.size() == 34U);
    REQUIRE(studioV1Preview.soundDirectoryEntryIndexByMpc60Pad.size() == 34U);

    REQUIRE(rockPreview.soundDirectoryEntryIndexByMpc60Pad[0] == 19);
    REQUIRE(rockPreview.soundDirectoryEntryIndexByMpc60Pad[1] == 20);
    REQUIRE(rockPreview.soundDirectoryEntryIndexByMpc60Pad[2] == 21);
    REQUIRE(rockPreview.soundDirectoryEntryIndexByMpc60Pad[10] == 16);

    REQUIRE(studioPreview.soundDirectoryEntryIndexByMpc60Pad[0] == 19);
    REQUIRE(studioPreview.soundDirectoryEntryIndexByMpc60Pad[1] == 20);
    REQUIRE(studioPreview.soundDirectoryEntryIndexByMpc60Pad[2] == 21);
    REQUIRE(studioPreview.soundDirectoryEntryIndexByMpc60Pad[10] == 23);

    REQUIRE(studioV1Preview.soundDirectoryEntryIndexByMpc60Pad[0] == 19);
    REQUIRE(studioV1Preview.soundDirectoryEntryIndexByMpc60Pad[1] == 20);
    REQUIRE(studioV1Preview.soundDirectoryEntryIndexByMpc60Pad[2] == 21);
    REQUIRE(studioV1Preview.soundDirectoryEntryIndexByMpc60Pad[10] == 23);

    REQUIRE(uk8Preview.soundDirectoryEntryIndexByMpc60Pad[0] == 0);
    REQUIRE(uk8Preview.soundDirectoryEntryIndexByMpc60Pad[18] == 255);
    REQUIRE(uk8Preview.soundDirectoryEntryIndexByMpc60Pad[25] == 255);
    REQUIRE(uk8Preview.soundDirectoryEntryIndexByMpc60Pad[33] == 20);

    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(0)->name == "HAT2CLSD");
    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(1)->name == "HAT2MED");
    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(2)->name == "HAT2OPN");
    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(10)->name == "RIDE4B");

    REQUIRE(studioPreview.assignedSoundAtMpc60Pad(0)->name == "HAT1CLSD");
    REQUIRE(studioPreview.assignedSoundAtMpc60Pad(1)->name == "HAT1MED");
    REQUIRE(studioPreview.assignedSoundAtMpc60Pad(2)->name == "HAT1OPN");
    REQUIRE(studioPreview.assignedSoundAtMpc60Pad(10)->name == "RIDE_#1");

    REQUIRE(studioV1Preview.assignedSoundAtMpc60Pad(0)->name == "HAT1CLSD");
    REQUIRE(studioV1Preview.assignedSoundAtMpc60Pad(1)->name == "HAT1MED");
    REQUIRE(studioV1Preview.assignedSoundAtMpc60Pad(2)->name == "HAT1OPN");
    REQUIRE(studioV1Preview.assignedSoundAtMpc60Pad(10)->name == "RIDE_#1");

    REQUIRE(uk8Preview.assignedSoundAtMpc60Pad(0)->name == "S7_HH_CL");
    REQUIRE(uk8Preview.assignedSoundAtMpc60Pad(10)->name == "S7_RIDE1");
    REQUIRE(uk8Preview.assignedSoundAtMpc60Pad(18) == nullptr);
    REQUIRE(uk8Preview.assignedSoundAtMpc60Pad(25) == nullptr);
    REQUIRE(uk8Preview.assignedSoundAtMpc60Pad(33)->name == "SCRATCH4");

    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(0)->isHihat);
    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(1)->isHihat);
    REQUIRE(rockPreview.assignedSoundAtMpc60Pad(2)->isHihat);
    REQUIRE_FALSE(rockPreview.assignedSoundAtMpc60Pad(16)->isHihat);

    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60PadName(0) == "hiht_clsd");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60PadName(10) == "rid1");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60PadName(33) == "dr16");

    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(0) == "A01");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(1) == "A01");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(2) == "A01");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(3) == "A02");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(17) == "A16");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(18) == "B01");
    REQUIRE(mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(33) == "B16");
}
