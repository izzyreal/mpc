#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "SampleOps.hpp"
#include "TestMpc.hpp"
#include "disk/MpcFile.hpp"
#include "disk/SoundLoader.hpp"
#include "file/kaitai/SndIo.hpp"
#include "file/kaitai/generated/mpc2000snd.h"
#include "file/sndreader/SndReader.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

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
using namespace mpc::file::sndreader;
using namespace mpc::file::sndwriter;
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

    std::shared_ptr<MpcFile> writeTempSndFile(const std::vector<char> &bytes,
                                              const std::string &fileName)
    {
        const auto tempDir = mpc_fs::temp_directory_path().value_or(mpc_fs::path{});
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
        auto file = writeTempSndFile(bytes, fileName);
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
}

TEST_CASE("Kaitai MPC2000 SND matches handwritten SND bytes", "[kaitai-snd]")
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

    SndWriter handwrittenWriter(&sound);
    const auto handwrittenBytes = handwrittenWriter.getSndFileArray();

    std::string kaitaiBytes;

    withParsedSndBytes(handwrittenBytes, [&](mpc2000snd_t &parsed)
    {
        REQUIRE(parsed.magic() == std::string("\x01\x04", 2));
        REQUIRE(parsed.name() == std::string(handwrittenBytes.begin() + 2, handwrittenBytes.begin() + 19));
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

        std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
        kaitai::kstream writeIo(&writeStream);
        parsed._set_io(&writeIo);
        parsed._check();
        parsed._write();

        kaitaiBytes = writeStream.str();
    });

    REQUIRE(kaitaiBytes == std::string(handwrittenBytes.begin(), handwrittenBytes.end()));

    std::vector<char> kaitaiBytesVec(kaitaiBytes.begin(), kaitaiBytes.end());
    SndReader handwrittenReader(kaitaiBytesVec);
    auto outputData = std::make_shared<std::vector<float>>();
    handwrittenReader.readData(outputData);

    REQUIRE(handwrittenReader.isHeaderValid());
    REQUIRE(handwrittenReader.getName() == "KICK_01");
    REQUIRE(handwrittenReader.isMono());
    REQUIRE(handwrittenReader.getLevel() == 100);
    REQUIRE(handwrittenReader.getTune() == -2);
    REQUIRE(handwrittenReader.getStart() == 0);
    REQUIRE(handwrittenReader.getEnd() == 2);
    REQUIRE(handwrittenReader.getLoopLength() == 1);
    REQUIRE(handwrittenReader.isLoopEnabled());
    REQUIRE(handwrittenReader.getNumberOfBeats() == 4);
    REQUIRE(handwrittenReader.getSampleRate() == 44100);
    REQUIRE(outputData->size() == inputData->size());
    REQUIRE((*outputData)[0] == (*inputData)[0]);
    REQUIRE((*outputData)[1] == (*inputData)[1]);
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

TEST_CASE("SndReader reads mutated header semantics from Kaitai-written bytes", "[kaitai-snd]")
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

    SndReader handwrittenReader(rewrittenBytes);
    auto outputData = std::make_shared<std::vector<float>>();
    handwrittenReader.readData(outputData);

    REQUIRE(handwrittenReader.isHeaderValid());
    REQUIRE(handwrittenReader.getName() == "mono_loop_mid");
    REQUIRE(handwrittenReader.isMono());
    REQUIRE(handwrittenReader.getSampleRate() == 44100);
    REQUIRE(handwrittenReader.getLevel() == 100);
    REQUIRE(handwrittenReader.getTune() == 120);
    REQUIRE(handwrittenReader.getStart() == 0);
    REQUIRE(handwrittenReader.getEnd() == 3);
    REQUIRE(handwrittenReader.getLoopLength() == 2);
    REQUIRE(!handwrittenReader.isLoopEnabled());
    REQUIRE(handwrittenReader.getNumberOfBeats() == 32);
    REQUIRE(outputData->size() == 4U);
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
