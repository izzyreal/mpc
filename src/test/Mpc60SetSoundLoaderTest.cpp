#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "SampleOps.hpp"
#include "TestMpc.hpp"
#include "TestMpc60SampleImport.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/Mpc60SampleImport.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "file/kaitai/Mpc60SetProgramLoader.hpp"
#include "file/kaitai/Mpc60SetSoundLoader.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"

#include <cmrc/cmrc.hpp>

#include <cstdint>
#include <string>
#include <vector>

CMRC_DECLARE(mpctest);

using mpc::test::ScopedMpc60ImportSetting;

namespace
{
    std::vector<char> resource(const std::string &path)
    {
        const auto fs = cmrc::mpctest::get_filesystem();
        const auto file = fs.open(path);
        return {file.begin(), file.end()};
    }

    std::vector<int16_t> expectedPcm()
    {
        const auto bytes = resource("test/GeneratedMpc60/Pcm/KICK1.pcm");
        std::vector<int16_t> result;
        result.reserve(bytes.size() / 2U);
        for (size_t i = 0; i < bytes.size(); i += 2)
        {
            const auto bits = static_cast<uint16_t>(
                static_cast<uint8_t>(bytes[i]) |
                (static_cast<uint16_t>(static_cast<uint8_t>(bytes[i + 1]))
                 << 8U));
            result.push_back(bits <= 0x7fffU ? static_cast<int16_t>(bits)
                                             : static_cast<int16_t>(
                                                   -1 - static_cast<int32_t>(
                                                            0xffffU - bits)));
        }
        return result;
    }

    void requireExpectedPcm(const mpc::sampler::Sound &sound)
    {
        const auto expected = expectedPcm();
        REQUIRE(sound.getSampleData()->size() == expected.size());
        for (size_t i = 0; i < expected.size(); ++i)
        {
            CAPTURE(i);
            REQUIRE(mpc::sampleops::mean_normalized_float_to_short(
                        sound.getSampleData()->at(i)) == expected[i]);
        }
    }
} // namespace

TEST_CASE("Generated MPC60 SET loads exact embedded sample PCM",
          "[kaitai-set][generated-mpc60]")
{
    ScopedMpc60ImportSetting enabled(true);
    const auto bytes = resource("test/GeneratedMpc60/Set/DEMO.SET");
    const auto preview =
        mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(bytes);
    REQUIRE(preview.totalNumberOfSampleWords == 14239);
    REQUIRE(preview.assignedSoundAtMpc60Pad(0) != nullptr);
    REQUIRE(preview.assignedSoundAtMpc60Pad(0)->name == "KICK1");

    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    const auto loaded =
        mpc::file::kaitai::Mpc60SetSoundLoader::loadAssignedSoundAtMpc60Pad(
            preview, 0, sound);
    REQUIRE(loaded.has_value());
    REQUIRE(sound->getName() == "KICK1");
    REQUIRE(sound->isMono());
    REQUIRE(sound->getSampleRate() == 44100);
    REQUIRE(sound->getFrameCount() == 14239);
    requireExpectedPcm(*sound);
}

TEST_CASE("Generated MPC60 SET loads through program integration",
          "[kaitai-set][generated-mpc60]")
{
    ScopedMpc60ImportSetting enabled(true);
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const auto bytes = resource("test/GeneratedMpc60/Set/DEMO.SET");
    auto file = std::make_shared<mpc::disk::MpcFile>(
        mpc.paths->getDocuments()->tempPath() / "DEMO.SET");
    auto writable = bytes;
    file->setFileData(writable);
    const auto preview =
        mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(bytes);

    REQUIRE(mpc::file::kaitai::Mpc60SetProgramLoader::load(
        mpc, file, preview,
        mpc::file::kaitai::Mpc60SetProgramLoader::defaultConversionTable(mpc),
        true));
    REQUIRE(mpc.getSampler()->getSoundCount() == 1);
    REQUIRE(mpc.getSampler()->getSoundName(0) == "KICK1");
    requireExpectedPcm(*mpc.getSampler()->getSound(0));
}

TEST_CASE("MPC60 SET kill switch rejects before parsing or mutation",
          "[kaitai-set][mpc60-kill-switch]")
{
    ScopedMpc60ImportSetting disabled(false);
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    sound->setName("unchanged");
    sound->getMutableSampleData()->push_back(
        mpc::sampleops::short_to_float(99));

    const auto loaded =
        mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
            std::vector<char>{0x02, 0x00}, 0, sound);
    REQUIRE_FALSE(loaded.has_value());
    REQUIRE(loaded.error() ==
            mpc::file::kaitai::kMpc60SetLoadingDisabledMessage);
    REQUIRE(sound->getName() == "unchanged");
    REQUIRE(sound->getSampleData()->size() == 1);
}
