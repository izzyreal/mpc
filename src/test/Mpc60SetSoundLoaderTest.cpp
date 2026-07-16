#include <catch2/catch_test_macros.hpp>

#include "SampleOps.hpp"
#include "file/kaitai/Mpc60SetSoundLoader.hpp"
#include "sampler/Sound.hpp"

#include <cmrc/cmrc.hpp>

#include <algorithm>
#include <cstdint>
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

    uint32_t readU4le(const std::vector<char> &bytes, const size_t offset)
    {
        return static_cast<uint32_t>(
            static_cast<unsigned char>(bytes[offset]) |
            (static_cast<unsigned char>(bytes[offset + 1]) << 8U) |
            (static_cast<unsigned char>(bytes[offset + 2]) << 16U) |
            (static_cast<unsigned char>(bytes[offset + 3]) << 24U));
    }

    std::vector<int16_t> readMpc2000xlSndPcm(const std::string &path)
    {
        const auto bytes = readResource(path);
        REQUIRE(bytes.size() >= 42);
        REQUIRE(static_cast<unsigned char>(bytes[0]) == 0x01);
        REQUIRE(static_cast<unsigned char>(bytes[1]) == 0x04);

        const auto stereo = (static_cast<unsigned char>(bytes[21]) & 1U) != 0;
        REQUIRE_FALSE(stereo);

        const auto frameCount = readU4le(bytes, 30);
        REQUIRE(bytes.size() == 42U + frameCount * 2U);

        std::vector<int16_t> result;
        result.reserve(frameCount);

        for (size_t offset = 42; offset < bytes.size(); offset += 2)
        {
            const auto value = static_cast<uint16_t>(
                static_cast<unsigned char>(bytes[offset]) |
                (static_cast<unsigned char>(bytes[offset + 1]) << 8U));
            result.push_back(static_cast<int16_t>(value));
        }

        return result;
    }

    void requireMatchesMpc2000xlSnd(const mpc::sampler::Sound &sound,
                                    const std::string &referencePath)
    {
        const auto expected = readMpc2000xlSndPcm(referencePath);
        const auto sampleData = sound.getSampleData();

        REQUIRE(sampleData->size() == expected.size());

        for (size_t i = 0; i < expected.size(); ++i)
        {
            REQUIRE(mpc::sampleops::mean_normalized_float_to_short(
                        sampleData->at(i)) == expected[i]);
        }
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

    const auto sampleData = sound->getSampleData();
    REQUIRE(sampleData->size() == 16000U);
    REQUIRE(std::any_of(sampleData->begin(), sampleData->end(),
                        [](const float sample)
                        {
                            return sample < 0.0f;
                        }));
    REQUIRE(std::any_of(sampleData->begin(), sampleData->end(),
                        [](const float sample)
                        {
                            return sample > 0.0f;
                        }));
    requireMatchesMpc2000xlSnd(
        *sound, "test/RealMpc60/SetImportedSnd/Rock/HAT2CLSD.SND");
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
    requireMatchesMpc2000xlSnd(
        *sound, "test/RealMpc60/SetImportedSnd/Rock/BIG_CLAP.SND");
}
