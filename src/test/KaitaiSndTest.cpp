#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "SampleOps.hpp"
#include "TestMpc.hpp"
#include "TestMpc60SampleImport.hpp"
#include "disk/MpcFile.hpp"
#include "disk/SoundLoader.hpp"
#include "file/kaitai/Mpc60SampleImport.hpp"
#include "file/kaitai/SndIo.hpp"
#include "file/kaitai/generated/mpc2000snd.h"
#include "file/kaitai/generated/mpc3000_snd_v2.h"
#include "file/kaitai/generated/mpc60_snd_v1.h"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

CMRC_DECLARE(mpctest);

using namespace mpc::disk;
using namespace mpc::file::kaitai;
using namespace mpc::file::kaitai::generated;
using namespace mpc::sampleops;
using mpc::test::ScopedMpc60ImportSetting;

namespace
{
    struct ExpectedSound
    {
        std::string name;
        bool mono;
        int sampleRate;
        int level;
        int tune;
        int beatCount;
        int start;
        int end;
        int loopTo;
        bool loopEnabled;
        std::vector<float> sampleData;
    };

    struct SaveRoundtripCase
    {
        std::string label;
        std::string fileStem;
        ExpectedSound expected;
    };

    std::vector<char> resourceBytes(const std::string &resourcePath)
    {
        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open(resourcePath);
        return std::vector<char>(file.begin(), file.end());
    }

    template <typename Assertions>
    void withParsedSndBytes(const std::vector<char> &bytes,
                            Assertions &&assertions)
    {
        std::istringstream parseStream(std::string(bytes.begin(), bytes.end()),
                                       std::ios::in | std::ios::binary);
        kaitai::kstream parseIo(&parseStream);
        mpc2000snd_t parsed(&parseIo);
        parsed._read();
        assertions(parsed);
    }

    template <typename Assertions>
    void withParsedMpc3000SndBytes(const std::vector<char> &bytes,
                                   Assertions &&assertions)
    {
        std::istringstream parseStream(std::string(bytes.begin(), bytes.end()),
                                       std::ios::in | std::ios::binary);
        kaitai::kstream parseIo(&parseStream);
        mpc3000_snd_v2_t parsed(&parseIo);
        parsed._read();
        assertions(parsed);
    }

    template <typename Assertions>
    void withParsedMpc60SndBytes(const std::vector<char> &bytes,
                                 Assertions &&assertions)
    {
        std::istringstream parseStream(std::string(bytes.begin(), bytes.end()),
                                       std::ios::in | std::ios::binary);
        kaitai::kstream parseIo(&parseStream);
        mpc60_snd_v1_t parsed(&parseIo);
        assertions(parsed);
    }

    std::shared_ptr<MpcFile> writeTempSndFile(mpc::Mpc &mpc,
                                              const std::vector<char> &bytes,
                                              const std::string &fileName)
    {
        const auto tempDir = mpc.paths->getDocuments()->tempPath();
        (void) mpc_fs::create_directories(tempDir);
        const auto path = tempDir / fileName;
        auto file = std::make_shared<MpcFile>(path);
        auto bytesCopy = bytes;
        file->setFileData(bytesCopy);
        return file;
    }

    std::shared_ptr<mpc::sampler::Sound> loadWithSoundLoader(mpc::Mpc &mpc,
                                                             const std::vector<char> &bytes,
                                                             const std::string &fileName)
    {
        auto file = writeTempSndFile(mpc, bytes, fileName);
        auto sound = mpc.getSampler()->addSound();
        SoundLoaderResult result;
        SoundLoader loader(mpc, false);
        loader.loadSound(file, result, sound, false);
        REQUIRE(result.success);
        return sound;
    }

    SoundLoaderResult loadWithSoundLoaderResult(
        mpc::Mpc &mpc,
        const std::vector<char> &bytes,
        const std::string &fileName,
        std::shared_ptr<mpc::sampler::Sound> &sound)
    {
        auto file = writeTempSndFile(mpc, bytes, fileName);
        sound = mpc.getSampler()->addSound();
        SoundLoaderResult result;
        SoundLoader loader(mpc, false);
        loader.loadSound(file, result, sound, false);
        return result;
    }

    void requireMpc60SndLoadingDisabled(mpc::Mpc &mpc,
                                        const std::vector<char> &bytes,
                                        const std::string &fileName)
    {
        std::shared_ptr<mpc::sampler::Sound> sound;
        const auto result =
            loadWithSoundLoaderResult(mpc, bytes, fileName, sound);

        REQUIRE_FALSE(result.success);
        REQUIRE(result.errorMessage ==
                ::mpc::file::kaitai::kMpc60SndLoadingDisabledMessage);
        REQUIRE(sound != nullptr);
        REQUIRE(sound->getSampleData()->empty());
    }

    std::vector<int16_t> pcmResource(const std::string &resourcePath)
    {
        const auto bytes = resourceBytes(resourcePath);
        REQUIRE((bytes.size() % 2U) == 0);
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

    void requireDecodedPcm(const mpc::sampler::Sound &sound,
                           const std::string &resourcePath)
    {
        const auto expected = pcmResource(resourcePath);
        const auto actual = sound.getSampleData();
        REQUIRE(actual->size() == expected.size());
        for (size_t i = 0; i < expected.size(); ++i)
        {
            CAPTURE(resourcePath, i);
            REQUIRE(mean_normalized_float_to_short(actual->at(i)) ==
                    expected[i]);
        }
    }

    void requireSoundMatches(const std::shared_ptr<mpc::sampler::Sound> &sound,
                             const ExpectedSound &expected)
    {
        REQUIRE(sound->getName() == expected.name);
        REQUIRE(sound->isMono() == expected.mono);
        REQUIRE(sound->getSampleRate() == expected.sampleRate);
        REQUIRE(sound->getSndLevel() == expected.level);
        REQUIRE(sound->getTune() == expected.tune);
        REQUIRE(sound->getBeatCount() == expected.beatCount);
        REQUIRE(sound->getStart() == expected.start);
        REQUIRE(sound->getEnd() == expected.end);
        REQUIRE(sound->getLoopTo() == expected.loopTo);
        REQUIRE(sound->isLoopEnabled() == expected.loopEnabled);
        REQUIRE(sound->getFrameCount() ==
                static_cast<int>(expected.sampleData.size() /
                                 (expected.mono ? 1 : 2)));
        REQUIRE(*sound->getSampleData() == expected.sampleData);
    }

    std::vector<float> monoSamples(const std::vector<int16_t> &samples)
    {
        std::vector<float> result;
        result.reserve(samples.size());
        for (const auto sample : samples)
        {
            result.push_back(short_to_float(sample));
        }
        return result;
    }

    std::vector<float> stereoSamples(const std::vector<int16_t> &left,
                                     const std::vector<int16_t> &right)
    {
        std::vector<float> result;
        result.reserve(left.size() + right.size());
        for (const auto sample : left)
        {
            result.push_back(short_to_float(sample));
        }
        for (const auto sample : right)
        {
            result.push_back(short_to_float(sample));
        }
        return result;
    }

    std::string expectedHeaderName(const std::string &name)
    {
        auto truncated = name.substr(0, 16);
        truncated.append(16 - truncated.size(), ' ');
        truncated.push_back('\0');
        return truncated;
    }

} // namespace

TEST_CASE("Kaitai MPC2000 SND saves and loads explicit semantics",
          "[kaitai-snd]")
{
    mpc::sampler::Sound sound(44100);
    sound.setName("KICK_01");
    sound.setMono(true);
    sound.setLevel(100);
    sound.setTune(-2);

    auto inputData = sound.getMutableSampleData();
    inputData->push_back(short_to_float(static_cast<int16_t>(1234)));
    inputData->push_back(short_to_float(static_cast<int16_t>(-1234)));

    sound.setStart(0);
    sound.setEnd(2);
    sound.setLoopTo(1);
    sound.setLoopEnabled(true);
    sound.setBeatCount(4);

    const auto kaitaiBytes = SndIo::saveSound(sound);

    withParsedSndBytes(kaitaiBytes, [&](mpc2000snd_t &parsed)
    {
        REQUIRE(parsed.magic() == std::string("\x01\x04", 2));
        REQUIRE(parsed.name() == expectedHeaderName("KICK_01"));
        REQUIRE(parsed.level() == 100);
        REQUIRE(parsed.tune() == -2);
        REQUIRE(parsed.stereo() == false);
        REQUIRE(parsed.start() == 0U);
        REQUIRE(parsed.end() == 2U);
        REQUIRE(parsed.frame_count() == 2U);
        REQUIRE(parsed.loop_frame_count() == 1U);
        REQUIRE(parsed.loop_enabled() == true);
        REQUIRE(parsed.beat_count() == 4);
        REQUIRE(parsed.sample_rate() == 44100);
        REQUIRE(parsed.sample_data() != nullptr);
        REQUIRE(parsed.sample_data()->size() == 2U);
        REQUIRE(parsed.sample_data()->at(0) == 1234);
        REQUIRE(parsed.sample_data()->at(1) == -1234);
    });

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    const auto loaded = loadWithSoundLoader(mpc, kaitaiBytes, "KICK_01.SND");
    requireSoundMatches(
        loaded,
        ExpectedSound{
            "KICK_01",
            true,
            44100,
            100,
            -2,
            4,
            0,
            2,
            1,
            true,
            *inputData
        });
}

TEST_CASE("Kaitai MPC2000 SND save/load covers prod-used sound properties", "[kaitai-snd]")
{
    const auto cases = std::vector<SaveRoundtripCase>{
        {
            "mono minimum bounds",
            "mono_min_bounds",
            ExpectedSound{
                "mono_min_bounds",
                true,
                0,
                0,
                -120,
                1,
                0,
                0,
                0,
                false,
                monoSamples({-32768, -1, 0, 32767})
            }
        },
        {
            "mono mid values",
            "mono_mid_values",
            ExpectedSound{
                "mono_mid_values",
                true,
                44100,
                100,
                0,
                16,
                1,
                3,
                2,
                true,
                monoSamples({-12345, -17, 19, 12345})
            }
        },
        {
            "stereo full span loop off",
            "stereo_full_span",
            ExpectedSound{
                "stereo_full_span",
                false,
                22050,
                100,
                0,
                16,
                0,
                5,
                0,
                false,
                stereoSamples({1000, 2000, 3000, 4000, 5000}, {-1000, -2000, -3000, -4000, -5000})
            }
        },
        {
            "stereo maximum bounds",
            "stereo_max_bounds",
            ExpectedSound{
                "stereo_max_bounds",
                false,
                65535,
                200,
                120,
                32,
                5,
                5,
                5,
                true,
                stereoSamples({32767, 23456, 12345, 1, -1}, {-32768, -23456, -12345, -1, 1})
            }
        }
    };

    for (const auto &testCase : cases)
    {
        SECTION(testCase.label)
        {
            mpc::sampler::Sound source(testCase.expected.sampleRate);
            source.setName(testCase.expected.name);
            source.setMono(testCase.expected.mono);
            source.setLevel(testCase.expected.level);
            source.setTune(testCase.expected.tune);
            source.setBeatCount(testCase.expected.beatCount);

            auto mutableData = source.getMutableSampleData();
            *mutableData = testCase.expected.sampleData;

            source.setStart(testCase.expected.start);
            source.setEnd(testCase.expected.end);
            source.setLoopTo(testCase.expected.loopTo);
            source.setLoopEnabled(testCase.expected.loopEnabled);

            const auto bytes = SndIo::saveSound(source);
            withParsedSndBytes(bytes, [&](mpc2000snd_t &parsed)
            {
                REQUIRE(parsed.level() == static_cast<uint8_t>(testCase.expected.level));
                REQUIRE(parsed.tune() == testCase.expected.tune);
                REQUIRE(parsed.stereo() == !testCase.expected.mono);
                REQUIRE(parsed.start() == static_cast<uint32_t>(testCase.expected.start));
                REQUIRE(parsed.end() == static_cast<uint32_t>(testCase.expected.end));
                REQUIRE(parsed.frame_count() == static_cast<uint32_t>(testCase.expected.sampleData.size() / (testCase.expected.mono ? 1 : 2)));
                REQUIRE(parsed.loop_frame_count() == static_cast<uint32_t>(testCase.expected.end - testCase.expected.loopTo));
                REQUIRE(parsed.loop_enabled() == testCase.expected.loopEnabled);
                REQUIRE(parsed.beat_count() == static_cast<uint8_t>(testCase.expected.beatCount));
                REQUIRE(parsed.sample_rate() == static_cast<uint16_t>(testCase.expected.sampleRate));
                REQUIRE(parsed.sample_data() != nullptr);
                REQUIRE(parsed.sample_data()->size() == testCase.expected.sampleData.size());

                for (std::size_t i = 0; i < parsed.sample_data()->size(); i++)
                {
                    REQUIRE(parsed.sample_data()->at(i) == mean_normalized_float_to_short(testCase.expected.sampleData[i]));
                }
            });

            mpc::Mpc mpc;
            mpc::TestMpc::initializeTestMpc(mpc);
            const auto loaded = loadWithSoundLoader(mpc, bytes, testCase.fileStem + ".SND");
            auto expectedLoaded = testCase.expected;
            expectedLoaded.name = expectedLoaded.name.substr(0, 16);
            requireSoundMatches(loaded, expectedLoaded);
        }
    }
}

TEST_CASE(
    "Kaitai MPC2000 SND parses real 2KXL mono and stereo files through the "
    "production loader",
    "[kaitai-snd][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto monoBytes =
        resourceBytes("test/RealMpc2000xl/Snd/mono_loop_off.SND");
    const auto mono = loadWithSoundLoader(mpc, monoBytes, "mono_loop_off.SND");

    REQUIRE(mono->getName() == "M");
    REQUIRE(mono->isMono());
    REQUIRE(mono->getSampleRate() == 44100);
    REQUIRE(mono->getSndLevel() == 100);
    REQUIRE(mono->getTune() == 0);
    REQUIRE(mono->getBeatCount() == 4);
    REQUIRE(mono->getStart() == 0);
    REQUIRE(mono->getEnd() == 8012);
    REQUIRE(mono->getLoopTo() == 0);
    REQUIRE(!mono->isLoopEnabled());
    REQUIRE(mono->getFrameCount() == 8012);
    REQUIRE(mono->getSampleData()->size() == 8012U);
    REQUIRE((*mono->getSampleData())[0] == short_to_float(-3326));
    REQUIRE((*mono->getSampleData())[1] == short_to_float(-2941));
    REQUIRE((*mono->getSampleData())[2] == short_to_float(-3809));

    const auto stereoBytes =
        resourceBytes("test/RealMpc2000xl/Snd/stereo_loop_off.SND");
    const auto stereo =
        loadWithSoundLoader(mpc, stereoBytes, "stereo_loop_off.SND");

    REQUIRE(stereo->getName() == "S");
    REQUIRE(!stereo->isMono());
    REQUIRE(stereo->getSampleRate() == 44100);
    REQUIRE(stereo->getSndLevel() == 100);
    REQUIRE(stereo->getTune() == 0);
    REQUIRE(stereo->getBeatCount() == 4);
    REQUIRE(stereo->getStart() == 0);
    REQUIRE(stereo->getEnd() == 8012);
    REQUIRE(stereo->getLoopTo() == 0);
    REQUIRE(!stereo->isLoopEnabled());
    REQUIRE(stereo->getFrameCount() == 8012);
    REQUIRE(stereo->getSampleData()->size() == 16024U);
    REQUIRE((*stereo->getSampleData())[0] == short_to_float(-3326));
    REQUIRE((*stereo->getSampleData())[1] == short_to_float(-2941));
    REQUIRE((*stereo->getSampleData())[2] == short_to_float(-3809));
    REQUIRE((*stereo->getSampleData())[8012] == short_to_float(-2233));
}

TEST_CASE("Kaitai MPC3000 SND parses a real hardware 01 02 file through the production loader", "[kaitai-snd][real-mpc3000]")
{
    const auto bytes = resourceBytes("test/RealMpc3000/Snd/SOUND017.SND");

    withParsedMpc3000SndBytes(bytes, [&](mpc3000_snd_v2_t &parsed)
    {
        REQUIRE(static_cast<unsigned char>(bytes[0]) == 0x01);
        REQUIRE(static_cast<unsigned char>(bytes[1]) == 0x02);
        REQUIRE(parsed.name() == expectedHeaderName("sound017").substr(0, 16));
        REQUIRE(parsed.level() == 100U);
        REQUIRE(parsed.unknown_1() == std::string(2, '\0'));
        REQUIRE(parsed.start() == 44U);
        REQUIRE(parsed.end() == 4410U);
        REQUIRE(parsed.frame_count() == 4410U);
        REQUIRE(parsed.sample_rate() == 44100);
        REQUIRE(parsed.header_size() == 38);
        REQUIRE(parsed.sample_data() != nullptr);
        REQUIRE(parsed.sample_data()->size() == 4410U);
        REQUIRE(parsed.sample_data()->at(0) == 5);
        REQUIRE(parsed.sample_data()->at(1) == 6);
        REQUIRE(parsed.sample_data()->at(2) == 7);
        REQUIRE(parsed.sample_data()->at(3) == 6);
    });

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    const auto loaded = loadWithSoundLoader(mpc, bytes, "SOUND017.SND");

    REQUIRE(loaded->getName() == "sound017");
    REQUIRE(loaded->isMono());
    REQUIRE(loaded->getSampleRate() == 44100);
    REQUIRE(loaded->getSndLevel() == 100);
    REQUIRE(loaded->getTune() == 0);
    REQUIRE(loaded->getBeatCount() == 4);
    REQUIRE(loaded->getStart() == 44);
    REQUIRE(loaded->getEnd() == 4410);
    REQUIRE(loaded->getLoopTo() == 0);
    REQUIRE_FALSE(loaded->isLoopEnabled());
    REQUIRE(loaded->getFrameCount() == 4410);
    REQUIRE(loaded->getSampleData()->size() == 4410U);
    REQUIRE((*loaded->getSampleData())[0] == short_to_float(5));
    REQUIRE((*loaded->getSampleData())[1] == short_to_float(6));
    REQUIRE((*loaded->getSampleData())[2] == short_to_float(7));
    REQUIRE((*loaded->getSampleData())[3] == short_to_float(6));
}

TEST_CASE("Generated MPC60 SND corpus decodes exact oracle PCM",
          "[kaitai-snd][generated-mpc60]")
{
    ScopedMpc60ImportSetting enabled(true);
    struct Fixture
    {
        const char *snd;
        const char *pcm;
        const char *name;
        size_t samples;
    };

    constexpr std::array<Fixture, 11> fixtures{{
        {"Snd/KICK1.SND", "KICK1.pcm", "KICK1", 14239},
        {"Snd/HAT1.SND", "HAT1.pcm", "HAT1", 12400},
        {"Snd/SNARE4.SND", "SNARE4.pcm", "SNARE4", 7215},
        {"Snd/OBOE.SND", "OBOE.pcm", "OBOE", 109842},
        {"Synthetic/one.SND", "one.pcm", "ONE", 1},
        {"Synthetic/two.SND", "two.pcm", "TWO", 2},
        {"Synthetic/three.SND", "three.pcm", "THREE", 3},
        {"Synthetic/transitions.SND", "transitions.pcm", "TRANSITIONS", 688},
        {"Synthetic/ramps.SND", "ramps.pcm", "RAMPS", 65538},
        {"Synthetic/boundary.SND", "boundary.pcm", "BOUNDARY", 1025},
        {"Synthetic/random.SND", "random.pcm", "RANDOM", 32769},
    }};

    for (const auto &fixture : fixtures)
    {
        DYNAMIC_SECTION(fixture.snd)
        {
            auto sound = std::make_shared<mpc::sampler::Sound>(44100);
            const auto result = SndIo::loadBytes(
                resourceBytes(std::string("test/GeneratedMpc60/") +
                              fixture.snd),
                sound, fixture.name);
            REQUIRE(result.has_value());
            REQUIRE(sound->getName() == fixture.name);
            REQUIRE(sound->isMono());
            REQUIRE(sound->getSampleRate() == 40000);
            REQUIRE(sound->getFrameCount() ==
                    static_cast<int>(fixture.samples));
            REQUIRE(sound->getStart() == 0);
            REQUIRE(sound->getEnd() == static_cast<int>(fixture.samples - 1));
            REQUIRE_FALSE(sound->isLoopEnabled());
            requireDecodedPcm(*sound, std::string("test/GeneratedMpc60/Pcm/") +
                                          fixture.pcm);
        }
    }
}

TEST_CASE("MPC60 SND kill switch rejects before parse and leaves sound intact",
          "[kaitai-snd][mpc60-kill-switch]")
{
    ScopedMpc60ImportSetting disabled(false);
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    sound->setName("unchanged");
    sound->getMutableSampleData()->push_back(short_to_float(123));

    const auto result = SndIo::loadBytes({0x01, 0x01}, sound, "TRUNCATED");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == kMpc60SndLoadingDisabledMessage);
    REQUIRE(sound->getName() == "unchanged");
    REQUIRE(sound->getSampleData()->size() == 1);
    REQUIRE(mean_normalized_float_to_short(sound->getSampleData()->front()) ==
            123);

    auto mpc3000 = std::make_shared<mpc::sampler::Sound>(44100);
    REQUIRE(SndIo::loadBytes(resourceBytes("test/RealMpc3000/Snd/SOUND017.SND"),
                             mpc3000, "SOUND017")
                .has_value());
    REQUIRE(mpc3000->getFrameCount() == 4410);
}

TEST_CASE("MPC60 SND uses production SoundLoader in both policy states",
          "[kaitai-snd][generated-mpc60][mpc60-kill-switch]")
{
    const auto bytes = resourceBytes("test/GeneratedMpc60/Snd/KICK1.SND");
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    {
        ScopedMpc60ImportSetting enabled(true);
        const auto loaded = loadWithSoundLoader(mpc, bytes, "KICK1.SND");
        REQUIRE(loaded->getName() == "KICK1");
        REQUIRE(loaded->getFrameCount() == 14239);
        requireDecodedPcm(*loaded, "test/GeneratedMpc60/Pcm/KICK1.pcm");
    }

    {
        ScopedMpc60ImportSetting disabled(false);
        requireMpc60SndLoadingDisabled(mpc, bytes, "KICK1.SND");
    }
}

TEST_CASE("MPC60 SND production load follows process environment",
          "[kaitai-snd][mpc60-environment]")
{
    ScopedMpc60ImportSetting environmentSetting(std::nullopt);
    const auto bytes = resourceBytes("test/GeneratedMpc60/Synthetic/three.SND");
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    const auto result = SndIo::loadBytes(bytes, sound, "THREE");

    const auto *environment =
        std::getenv("VMPC2000XL_DISABLE_MPC60_SAMPLE_IMPORT");
    if (environment != nullptr && std::string_view(environment) == "1")
    {
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == kMpc60SndLoadingDisabledMessage);
        REQUIRE(sound->getSampleData()->empty());
    }
    else
    {
        REQUIRE(result.has_value());
        REQUIRE(sound->getFrameCount() == 3);
    }
}

TEST_CASE("Malformed MPC60 SND fails transactionally when enabled",
          "[kaitai-snd][generated-mpc60]")
{
    ScopedMpc60ImportSetting enabled(true);
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    sound->setName("unchanged");
    sound->getMutableSampleData()->push_back(short_to_float(-321));

    const auto result = SndIo::loadBytes({0x01, 0x01}, sound, "TRUNCATED");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(sound->getName() == "unchanged");
    REQUIRE(sound->getSampleData()->size() == 1);
    REQUIRE(mean_normalized_float_to_short(sound->getSampleData()->front()) ==
            -321);
}

TEST_CASE("SoundLoader uses the SND header name at byte offset 0x02", "[kaitai-snd]")
{
    auto bytes =
        resourceBytes("test/RealMpc3000/Snd/SOUND017.SND");
    const auto headerName = expectedHeaderName("HeaderName");
    std::copy(headerName.begin(), headerName.end(), bytes.begin() + 0x02);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    const auto loaded = loadWithSoundLoader(mpc, bytes, "UNRELATED.SND");

    REQUIRE(loaded->getName() == "HeaderName");
}

TEST_CASE("Generated MPC2000XL SND corpus loads through the production loader",
          "[kaitai-snd][generated-corpus]")
{
    struct CorpusCase
    {
        std::string resourcePath;
        std::string fileName;
        ExpectedSound expected;
    };

    const auto cases = std::vector<CorpusCase>{
        {
            "test/GeneratedMpc2000xl/Snd/mono_corpus_min.SND",
            "mono_corpus_min.SND",
            ExpectedSound{
                "mono_corpus_min",
                true,
                8000,
                0,
                -120,
                1,
                0,
                0,
                0,
                false,
                monoSamples({-32768, -1, 0, 32767})
            }
        },
        {
            "test/GeneratedMpc2000xl/Snd/mono_loop_mid.SND",
            "mono_loop_mid.SND",
            ExpectedSound{
                "mono_loop_mid",
                true,
                44100,
                100,
                0,
                16,
                1,
                3,
                2,
                true,
                monoSamples({-12345, -17, 19, 12345})
            }
        },
        {
            "test/GeneratedMpc2000xl/Snd/stereo_span.SND",
            "stereo_span.SND",
            ExpectedSound{
                "stereo_span",
                false,
                22050,
                100,
                0,
                16,
                0,
                5,
                0,
                false,
                stereoSamples({1000, 2000, 3000, 4000, 5000}, {-1000, -2000, -3000, -4000, -5000})
            }
        },
        {
            "test/GeneratedMpc2000xl/Snd/stereo_max.SND",
            "stereo_max.SND",
            ExpectedSound{
                "stereo_max",
                false,
                44100,
                200,
                120,
                32,
                5,
                5,
                5,
                true,
                stereoSamples({32767, 23456, 12345, 1, -1}, {-32768, -23456, -12345, -1, 1})
            }
        },
        {
            "test/GeneratedMpc2000xl/Snd/name_len_sixteen.SND",
            "name_len_sixteen.SND",
            ExpectedSound{
                "name_len_sixteen",
                false,
                32000,
                150,
                -37,
                8,
                2,
                4,
                2,
                true,
                stereoSamples({3210, -4321, 123, -456}, {999, -999, 456, -123})
            }
        }
    };

    for (const auto &testCase : cases)
    {
        SECTION(testCase.fileName)
        {
            const auto bytes = resourceBytes(testCase.resourcePath);

            withParsedSndBytes(bytes, [&](mpc2000snd_t &parsed)
            {
                REQUIRE(parsed.name() == expectedHeaderName(testCase.expected.name));
                REQUIRE(parsed.level() == static_cast<uint8_t>(testCase.expected.level));
                REQUIRE(parsed.tune() == testCase.expected.tune);
                REQUIRE(parsed.stereo() == !testCase.expected.mono);
                REQUIRE(parsed.start() == static_cast<uint32_t>(testCase.expected.start));
                REQUIRE(parsed.end() == static_cast<uint32_t>(testCase.expected.end));
                REQUIRE(parsed.frame_count() == static_cast<uint32_t>(testCase.expected.sampleData.size() / (testCase.expected.mono ? 1 : 2)));
                REQUIRE(parsed.loop_frame_count() == static_cast<uint32_t>(testCase.expected.end - testCase.expected.loopTo));
                REQUIRE(parsed.loop_enabled() == testCase.expected.loopEnabled);
                REQUIRE(parsed.beat_count() == static_cast<uint8_t>(testCase.expected.beatCount));
                REQUIRE(parsed.sample_rate() == static_cast<uint16_t>(testCase.expected.sampleRate));
                REQUIRE(parsed.sample_data() != nullptr);
                REQUIRE(parsed.sample_data()->size() == testCase.expected.sampleData.size());

                for (std::size_t i = 0; i < parsed.sample_data()->size(); i++)
                {
                    REQUIRE(parsed.sample_data()->at(i) == mean_normalized_float_to_short(testCase.expected.sampleData[i]));
                }
            });

            mpc::Mpc mpc;
            mpc::TestMpc::initializeTestMpc(mpc);
            const auto loaded = loadWithSoundLoader(mpc, bytes, testCase.fileName);
            requireSoundMatches(loaded, testCase.expected);
        }
    }
}

TEST_CASE("SndIo reads mutated header semantics from Kaitai-written bytes", "[kaitai-snd]")
{
    auto bytes = resourceBytes("test/GeneratedMpc2000xl/Snd/mono_loop_mid.SND");

    std::vector<char> rewrittenBytes;
    withParsedSndBytes(bytes, [&](mpc2000snd_t &parsed)
    {
        parsed.set_tune(120);
        parsed.set_start(0);
        parsed.set_end(3);
        parsed.set_loop_frame_count(2);
        parsed.set_loop_enabled(false);
        parsed.set_beat_count(32);

        std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
        kaitai::kstream writeIo(&writeStream);
        parsed._set_io(&writeIo);
        parsed._check();
        parsed._write();

        const auto written = writeStream.str();
        rewrittenBytes.assign(written.begin(), written.end());
    });

    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    auto result = SndIo::loadBytes(rewrittenBytes, sound, "mono_loop_mid");
    REQUIRE(result);
    REQUIRE(sound->getName() == "mono_loop_mid");
    REQUIRE(sound->isMono());
    REQUIRE(sound->getSampleRate() == 44100);
    REQUIRE(sound->getSndLevel() == 100);
    REQUIRE(sound->getTune() == 120);
    REQUIRE(sound->getStart() == 0);
    REQUIRE(sound->getEnd() == 3);
    REQUIRE(sound->getLoopTo() == 1);
    REQUIRE(!sound->isLoopEnabled());
    REQUIRE(sound->getBeatCount() == 32);
    REQUIRE(sound->getSampleData()->size() == 4U);
}

TEST_CASE("SoundLoader loads mutated SND header semantics from Kaitai-written bytes", "[kaitai-snd]")
{
    auto bytes = resourceBytes("test/GeneratedMpc2000xl/Snd/stereo_span.SND");

    std::vector<char> rewrittenBytes;
    withParsedSndBytes(bytes, [&](mpc2000snd_t &parsed)
    {
        parsed.set_tune(-120);
        parsed.set_start(1);
        parsed.set_end(4);
        parsed.set_loop_frame_count(1);
        parsed.set_loop_enabled(true);
        parsed.set_beat_count(1);

        std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
        kaitai::kstream writeIo(&writeStream);
        parsed._set_io(&writeIo);
        parsed._check();
        parsed._write();

        const auto written = writeStream.str();
        rewrittenBytes.assign(written.begin(), written.end());
    });

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    const auto loaded = loadWithSoundLoader(mpc, rewrittenBytes, "stereo_span.SND");

    REQUIRE(loaded->getName() == "stereo_span");
    REQUIRE(!loaded->isMono());
    REQUIRE(loaded->getTune() == -120);
    REQUIRE(loaded->getStart() == 1);
    REQUIRE(loaded->getEnd() == 4);
    REQUIRE(loaded->getLoopTo() == 3);
    REQUIRE(loaded->isLoopEnabled());
    REQUIRE(loaded->getBeatCount() == 1);
}

TEST_CASE("SndIo rejects too-short and unsupported SND data", "[kaitai-snd]")
{
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);

    auto tooShort = SndIo::loadBytes({static_cast<char>(0x01)}, sound, "bad");
    REQUIRE_FALSE(tooShort);
    REQUIRE(tooShort.error() == "SND file is too short");

    auto unsupported = SndIo::loadBytes(
        {static_cast<char>(0x01), static_cast<char>(0x03)}, sound, "bad");
    REQUIRE_FALSE(unsupported);
    REQUIRE(unsupported.error() == "Unsupported SND file format");
}
