#include <catch2/catch_test_macros.hpp>

#include "file/kaitai/Mpc60SetPreview.hpp"
#include "file/kaitai/Mpc60SetSoundLoader.hpp"
#include "sampler/Sound.hpp"

#include <cmrc/cmrc.hpp>

#include <algorithm>
#include <string>
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

    void requireSilentSamples(const mpc::sampler::Sound &sound,
                              const std::size_t expectedSize)
    {
        const auto sampleData = sound.getSampleData();

        REQUIRE(sampleData->size() == expectedSize);
        REQUIRE(std::all_of(sampleData->begin(), sampleData->end(),
                            [](const float sample)
                            {
                                return sample == 0.0f;
                            }));
    }
} // namespace

TEST_CASE("MPC60 SET single-sound loader decodes the default assigned sound",
          "[kaitai-set][real-mpc60]")
{
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);

    const auto result =
        mpc::file::kaitai::Mpc60SetSoundLoader::loadAssignedSoundAtMpc60Pad(
            readResource("test/RealMpc60/SetV0/ROCK.SET"), 0, sound);

    REQUIRE(result.has_value());
    REQUIRE(sound->getName() == "HAT2CLSD");
    REQUIRE(sound->isMono());
    REQUIRE(sound->getSampleRate() == 44100);
    REQUIRE(sound->getTune() == -17);
    REQUIRE(sound->getFrameCount() == 16000);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 15999);
    REQUIRE_FALSE(sound->isLoopEnabled());

    requireSilentSamples(*sound, 16000U);
}

TEST_CASE("MPC60 SET single-sound loader decodes a selected directory entry",
          "[kaitai-set][real-mpc60]")
{
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);

    const auto result =
        mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
            readResource("test/RealMpc60/SetV0/ROCK.SET"), 17, sound);

    REQUIRE(result.has_value());
    REQUIRE(sound->getName() == "BIG_CLAP");
    REQUIRE(sound->getFrameCount() == 24000);
    REQUIRE(sound->getEnd() == 23999);
    requireSilentSamples(*sound, 24000U);
}

TEST_CASE("MPC60 SET preview supports multiple sound loads without reparsing",
          "[kaitai-set][real-mpc60]")
{
    const auto preview = mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(
        readResource("test/RealMpc60/SetV0/ROCK.SET"));
    const auto previewCopy = preview;
    REQUIRE(previewCopy.soundSampleWords == preview.soundSampleWords);

    auto closedHat = std::make_shared<mpc::sampler::Sound>(44100);
    auto clap = std::make_shared<mpc::sampler::Sound>(44100);
    REQUIRE(mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
                preview, 19, closedHat)
                .has_value());
    REQUIRE(mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
                preview, 17, clap)
                .has_value());
    REQUIRE(closedHat->getName() == "HAT2CLSD");
    REQUIRE(closedHat->getFrameCount() == 16000U);
    REQUIRE(clap->getName() == "BIG_CLAP");
    REQUIRE(clap->getFrameCount() == 24000U);
}

TEST_CASE("MPC60 SET preview loader rejects an invalid sound sample range",
          "[kaitai-set]")
{
    auto preview = mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(
        readResource("test/RealMpc60/SetV0/ROCK.SET"));
    preview.soundDirectoryEntries[0].startAddressInMemory =
        preview.totalNumberOfSampleWords;
    preview.soundDirectoryEntries[0].lengthInSamples = 1;

    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    const auto result =
        mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
            preview, 0, sound);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "SET sound sample range out of bounds");
}
