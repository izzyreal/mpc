#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "SampleOps.hpp"
#include "TestMpc.hpp"
#include "disk/MpcFile.hpp"
#include "disk/SoundLoader.hpp"
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
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

CMRC_DECLARE(mpctest);

using namespace mpc::disk;
using namespace mpc::file::kaitai;
using namespace mpc::file::kaitai::generated;
using namespace mpc::sampleops;

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
    void withParsedSndBytes(const std::vector<char> &bytes, Assertions &&assertions)
    {
        std::stringstream parseStream(
            std::string(bytes.begin(), bytes.end()),
            std::ios::in | std::ios::out | std::ios::binary
        );
        kaitai::kstream parseIo(&parseStream);
        mpc2000snd_t parsed(&parseIo);
        parsed._read();
        assertions(parsed);
    }

    template <typename Assertions>
    void withParsedMpc3000SndBytes(const std::vector<char> &bytes, Assertions &&assertions)
    {
        std::stringstream parseStream(
            std::string(bytes.begin(), bytes.end()),
            std::ios::in | std::ios::out | std::ios::binary
        );
        kaitai::kstream parseIo(&parseStream);
        mpc3000_snd_v2_t parsed(&parseIo);
        parsed._read();
        assertions(parsed);
    }

    template <typename Assertions>
    void withParsedMpc60SndBytes(const std::vector<char> &bytes, Assertions &&assertions)
    {
        std::stringstream parseStream(
            std::string(bytes.begin(), bytes.end()),
            std::ios::in | std::ios::out | std::ios::binary
        );
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
        REQUIRE(sound->getFrameCount() == static_cast<int>(expected.sampleData.size() / (expected.mono ? 1 : 2)));
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

    uint64_t fnv1aPcmHash(const std::vector<float> &samples)
    {
        uint64_t hash = 1469598103934665603ULL;
        for (const auto sample : samples)
        {
            const auto pcm = static_cast<uint16_t>(mean_normalized_float_to_short(sample));
            hash ^= (pcm & 0xffU);
            hash *= 1099511628211ULL;
            hash ^= ((pcm >> 8U) & 0xffU);
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    void requirePcmWindow(const std::vector<float> &samples,
                          const std::size_t offset,
                          const std::array<int16_t, 4> &expected)
    {
        REQUIRE(samples.size() >= offset + expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            REQUIRE(mean_normalized_float_to_short(samples[offset + i]) ==
                    expected[i]);
        }
    }

}

TEST_CASE("Kaitai MPC2000 SND saves and loads explicit semantics", "[kaitai-snd]")
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
            requireSoundMatches(loaded, testCase.expected);
        }
    }
}

TEST_CASE("Kaitai MPC2000 SND parses real 2KXL mono and stereo files through the production loader", "[kaitai-snd][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto monoBytes = resourceBytes("test/RealMpc2000xl/Snd/mono_loop_off.SND");
    const auto mono = loadWithSoundLoader(mpc, monoBytes, "mono_loop_off.SND");

    REQUIRE(mono->getName() == "mono_loop_off");
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

    const auto stereoBytes = resourceBytes("test/RealMpc2000xl/Snd/stereo_loop_off.SND");
    const auto stereo = loadWithSoundLoader(mpc, stereoBytes, "stereo_loop_off.SND");

    REQUIRE(stereo->getName() == "stereo_loop_off");
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

TEST_CASE("Kaitai MPC60 SND parses a real hardware 01 01 file through the production loader", "[kaitai-snd][real-mpc60]")
{
    const auto bytes = resourceBytes("test/RealMpc60/Snd/SOUND002.SND");

    withParsedMpc60SndBytes(bytes, [&](mpc60_snd_v1_t &parsed)
    {
        REQUIRE(static_cast<unsigned char>(bytes[0]) == 0x01);
        REQUIRE(static_cast<unsigned char>(bytes[1]) == 0x01);
        REQUIRE(parsed.name() == expectedHeaderName("sound002").substr(0, 16));
        REQUIRE(parsed.sample_count() == 4000U);
        REQUIRE(parsed.time_from_start_of_data_to_end_of_play_msec() == 99U);
        REQUIRE(parsed.time_from_start_of_data_to_start_play_msec() == 20U);
        REQUIRE(parsed.decay_time_msec() == 20U);
        REQUIRE(parsed.volume_percent() == 100U);
        REQUIRE(parsed.tuning() == 0);
        REQUIRE(parsed.reserved_zeroes() == std::string(5, '\0'));
        REQUIRE(parsed.velocity_to_volume_percent() == 100U);
        REQUIRE(parsed.sample_rate() == 40000);
        REQUIRE(parsed.sample_data_pairs() != nullptr);
        REQUIRE(parsed.sample_data_pairs()->size() == 2000U);
    });

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    const auto loaded = loadWithSoundLoader(mpc, bytes, "SOUND002.SND");

    REQUIRE(loaded->getName() == "sound002");
    REQUIRE(loaded->isMono());
    REQUIRE(loaded->getSampleRate() == 40000);
    REQUIRE(loaded->getSndLevel() == 100);
    REQUIRE(loaded->getTune() == 0);
    REQUIRE(loaded->getBeatCount() == 4);
    REQUIRE(loaded->getStart() == 0);
    REQUIRE(loaded->getEnd() == 3999);
    REQUIRE(loaded->getLoopTo() == 0);
    REQUIRE_FALSE(loaded->isLoopEnabled());
    REQUIRE(loaded->getFrameCount() == 4000);
    REQUIRE(loaded->getSampleData()->size() == 4000U);
    const auto [minIt, maxIt] = std::minmax_element(
        loaded->getSampleData()->begin(), loaded->getSampleData()->end());
    CAPTURE(*minIt, *maxIt);
    REQUIRE(*minIt < *maxIt);
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

TEST_CASE("Kaitai MPC60 SND parses native MPC60 2.14 ROCK exports through the production loader", "[kaitai-snd][real-mpc60]")
{
    struct NativeMpc60SndCase
    {
        std::string fileName;
        std::string headerName;
        std::string loadedName;
        uint32_t playbackEndMsec;
        uint16_t decayTimeMsec;
        uint64_t decodedPcmHash;
        std::array<int16_t, 4> firstPcm;
        std::array<int16_t, 4> middlePcm;
        std::array<int16_t, 4> lastPcm;
    };

    const auto files = std::vector<NativeMpc60SndCase>{
        {"BASSGUIT.SND", "BASSGUIT", "BASSGUIT", 200, 50, 0xa5d8d39839ca44abULL, {162, 315, 446, 557}, {1717, 1169, 184, -1198}, {-5648, -5480, -5297, -5104}},
        {"BIG_CLAP.SND", "BIG_CLAP", "BIG_CLAP", 600, 350, 0xdea861a215d6edc7ULL, {6, 11, 14, 9}, {-1066, -880, -632, -212}, {0, 0, 0, 0}},
        {"COWBEL_1.SND", "COWBEL#1", "COWBEL_1", 350, 100, 0xbc117cd727a9f5b2ULL, {27, 52, 68, 72}, {18, 134, 124, 25}, {234, 211, 209, 202}},
        {"CRASH2_H.SND", "CRASH2_H", "CRASH2_H", 995, 163, 0x2b4a71d36d7733b7ULL, {-11666, -2611, -1215, -748}, {-709, -1140, -1363, 479}, {1127, 1296, 445, -219}},
        {"CRASH2_L.SND", "CRASH2_L", "CRASH2_L", 1766, 261, 0xfa4c815ae3824e34ULL, {-11666, -2611, -1208, -737}, {-661, -1047, -816, -180}, {191, 225, 207, 166}},
        {"CRASH_MU.SND", "CRASH_MUTE", "CRASH_MU", 634, 117, 0x7e2aba841084908fULL, {13, 9, 19, 12}, {-5, 175, 229, 183}, {58, 38, 30, 26}},
        {"GONG.SND", "GONG", "GONG", 1310, 148, 0x377926352459736fULL, {-11761, -2336, -1049, -824}, {1663, 1547, 1340, 1238}, {203, 169, 134, 173}},
        {"HAT2CLSD.SND", "HAT2CLSD", "HAT2CLSD", 400, 100, 0x4ca36b849f85e99cULL, {27, 52, 55, 69}, {674, 18, 1, -52}, {72, 87, 84, 81}},
        {"HAT2MED.SND", "HAT2MED", "HAT2MED", 600, 100, 0xe513d30847955496ULL, {33, 57, 58, 64}, {1164, 1296, 941, -435}, {390, 304, 6, 114}},
        {"HAT2OPN.SND", "HAT2OPN", "HAT2OPN", 1000, 200, 0x21b0c6dba5e0f595ULL, {27, 45, 57, 64}, {-1474, -776, -1336, -288}, {371, 307, 430, 377}},
        {"KICK_2.SND", "KiCK#2", "KiCK_2", 400, 100, 0x1b017ff1ffe6524bULL, {13, 22, 28, 32}, {293, 265, 244, 13}, {-181, -206, -235, -280}},
        {"METALSHO.SND", "METALSHOT", "METALSHO", 269, 131, 0xf1529a4cd4fb64ecULL, {-11612, -2675, -1336, -862}, {-169, -402, -750, -896}, {-75, -67, -70, -72}},
        {"RIDE4B.SND", "RIDE4B", "RIDE4B", 961, 172, 0xd07be7c6d058175fULL, {2551, -5462, -4310, -2054}, {-839, -522, -213, -104}, {-106, -116, -115, -87}},
        {"ROCTOMID.SND", "ROCTOMID", "ROCTOMID", 800, 350, 0x2376e1fe2307dba6ULL, {13, 16, 24, 29}, {827, 815, 794, 774}, {39, 46, 43, 41}},
        {"ROCTOMLO.SND", "ROCTOMLO", "ROCTOMLO", 1500, 900, 0x6ba2aad9ea269cadULL, {0, -6, -4, -8}, {-347, -372, -396, -411}, {-17, -17, -11, -6}},
        {"SIDESTK2.SND", "SIDESTK2", "SIDESTK2", 400, 100, 0x1c938199fd48368bULL, {-95, -160, -213, -254}, {17, -57, -134, -82}, {-44, -55, -49, -51}},
        {"SNARE_2.SND", "SNARE#2", "SNARE_2", 500, 100, 0xe36c4c5128b7dd18ULL, {6, 11, 14, 9}, {138, -22, 535, 878}, {158, 212, 254, 280}},
    };

    for (const auto &testCase : files)
    {
        SECTION(testCase.fileName)
        {
            const auto resourcePath =
                "test/RealMpc60/Snd/Mpc60V214Rock/" + testCase.fileName;
            const auto bytes = resourceBytes(resourcePath);
            const auto fileStem = testCase.fileName.substr(0, testCase.fileName.size() - 4);
            const auto expectedSampleCount =
                static_cast<uint32_t>((bytes.size() - 39U) * 2U / 3U);

            withParsedMpc60SndBytes(bytes, [&](mpc60_snd_v1_t &parsed)
            {
                REQUIRE(static_cast<unsigned char>(bytes[0]) == 0x01);
                REQUIRE(static_cast<unsigned char>(bytes[1]) == 0x01);
                REQUIRE(parsed.name() == expectedHeaderName(testCase.headerName).substr(0, 16));
                REQUIRE(parsed.sample_count() == expectedSampleCount);
                REQUIRE(parsed.packed_sample_data_byte_count() ==
                        static_cast<int32_t>(bytes.size() - 39U));
                REQUIRE(parsed.sample_rate() == 40000);
                REQUIRE(parsed.time_from_start_of_data_to_start_play_msec() == 0U);
                REQUIRE(parsed.time_from_start_of_data_to_end_of_play_msec() ==
                        testCase.playbackEndMsec);
                REQUIRE(parsed.decay_time_msec() == testCase.decayTimeMsec);
                REQUIRE(parsed.volume_percent() == 100U);
                REQUIRE(parsed.tuning() == 0);
                REQUIRE(parsed.reserved_zeroes() == std::string(5, '\0'));
                REQUIRE(parsed.velocity_to_volume_percent() == 100U);
                REQUIRE(parsed.sample_data_pairs() != nullptr);
                REQUIRE(parsed.sample_data_pairs()->size() ==
                        static_cast<std::size_t>((expectedSampleCount + 1U) / 2U));
            });

            mpc::Mpc mpc;
            mpc::TestMpc::initializeTestMpc(mpc);
            const auto loaded = loadWithSoundLoader(mpc, bytes, testCase.fileName);

            REQUIRE(loaded->getName() == testCase.loadedName);
            REQUIRE(loaded->isMono());
            REQUIRE(loaded->getSampleRate() == 40000);
            REQUIRE(loaded->getSndLevel() == 100);
            REQUIRE(loaded->getTune() == 0);
            REQUIRE(loaded->getBeatCount() == 4);
            REQUIRE(loaded->getStart() == 0);
            REQUIRE(loaded->getEnd() == static_cast<int>(expectedSampleCount) - 1);
            REQUIRE(loaded->getLoopTo() == 0);
            REQUIRE_FALSE(loaded->isLoopEnabled());
            REQUIRE(loaded->getFrameCount() == static_cast<int>(expectedSampleCount));
            REQUIRE(loaded->getSampleData()->size() == expectedSampleCount);

            const auto &samples = *loaded->getSampleData();
            const auto [minIt, maxIt] = std::minmax_element(
                samples.begin(), samples.end());
            CAPTURE(testCase.fileName, *minIt, *maxIt);
            REQUIRE(*minIt < *maxIt);
            REQUIRE(fnv1aPcmHash(samples) == testCase.decodedPcmHash);
            requirePcmWindow(samples, 0, testCase.firstPcm);
            requirePcmWindow(samples, samples.size() / 2, testCase.middlePcm);
            requirePcmWindow(samples, samples.size() - testCase.lastPcm.size(),
                             testCase.lastPcm);
        }
    }
}

TEST_CASE("Generated MPC2000XL SND corpus loads through the production loader", "[kaitai-snd][generated-corpus]")
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
        {static_cast<char>(0x01), static_cast<char>(0x03)}, sound,
        "bad");
    REQUIRE_FALSE(unsupported);
    REQUIRE(unsupported.error() == "Unsupported SND file format");
}
