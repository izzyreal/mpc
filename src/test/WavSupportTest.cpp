#include <catch2/catch_test_macros.hpp>

#include "SampleOps.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "audiomidi/WavInputFileStream.hpp"
#include "engine/audio/core/AudioBuffer.hpp"
#include "file/wav/WavFile.hpp"

#include <cmrc/cmrc.hpp>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace mpc::audiomidi;
using namespace mpc::engine::audio::core;
using namespace mpc::file::wav;
using namespace mpc::sampleops;

CMRC_DECLARE(mpctest);

namespace
{
    std::vector<char> resourceBytes(const std::string &resourcePath)
    {
        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open(resourcePath);
        return std::vector<char>(file.begin(), file.end());
    }

    void appendLe(std::string &bytes, uint32_t value, int byteCount)
    {
        for (int i = 0; i < byteCount; i++)
        {
            bytes.push_back(static_cast<char>((value >> (i * 8)) & 0xFF));
        }
    }

    void appendFloat32(std::string &bytes, float value)
    {
        uint32_t bits = 0;
        static_assert(sizeof(bits) == sizeof(value));
        std::memcpy(&bits, &value, sizeof(value));
        appendLe(bytes, bits, 4);
    }

    std::shared_ptr<std::stringstream>
    makeWavStream(uint16_t formatCode, uint16_t numChannels,
                  uint32_t sampleRate, uint16_t bitsPerSample,
                  const std::vector<uint32_t> &pcmFrames,
                  const std::vector<float> &floatFrames = {})
    {
        std::string bytes;
        const auto bytesPerSample = bitsPerSample / 8;
        const auto frameCount = pcmFrames.empty() ? floatFrames.size()
                                                  : pcmFrames.size();
        const auto dataChunkSize =
            static_cast<uint32_t>(frameCount * bytesPerSample * numChannels);
        const auto riffChunkSize = 36 + dataChunkSize;

        bytes.reserve(riffChunkSize + 8);
        bytes.append("RIFF", 4);
        appendLe(bytes, riffChunkSize, 4);
        bytes.append("WAVE", 4);
        bytes.append("fmt ", 4);
        appendLe(bytes, 16, 4);
        appendLe(bytes, formatCode, 2);
        appendLe(bytes, numChannels, 2);
        appendLe(bytes, sampleRate, 4);
        appendLe(bytes, sampleRate * numChannels * bytesPerSample, 4);
        appendLe(bytes, numChannels * bytesPerSample, 2);
        appendLe(bytes, bitsPerSample, 2);
        bytes.append("data", 4);
        appendLe(bytes, dataChunkSize, 4);

        if (formatCode == 3)
        {
            for (const auto sample : floatFrames)
            {
                appendFloat32(bytes, sample);
            }
        }
        else
        {
            for (const auto sample : pcmFrames)
            {
                appendLe(bytes, sample, bytesPerSample);
            }
        }

        return std::make_shared<std::stringstream>(
            std::string(bytes.data(), bytes.size()),
            std::ios::in | std::ios::out | std::ios::binary);
    }
} // namespace

TEST_CASE("32-bit float WAV files are accepted and decoded", "[wav]")
{
    auto stream = makeWavStream(3, 1, 44100, 32, {}, {0.25f, -0.25f});

    auto wavOrError = WavFile::readWavStream(stream);
    REQUIRE(wavOrError.has_value());

    std::vector<float> frames;
    REQUIRE(wavOrError.value()->readFrames(frames, 2) == 2);
    REQUIRE(frames.size() == 2);
    REQUIRE(std::fabs(frames[0] - 0.25f) < 0.0001f);
    REQUIRE(std::fabs(frames[1] + 0.25f) < 0.0001f);
}

TEST_CASE("32-bit PCM WAV preview reads full-width samples", "[wav][preview]")
{
    auto stream = makeWavStream(1, 1, 44100, 32,
                                {0x40000000u, 0xC0000000u, 0x00000000u});

    int sampleRate = 0;
    int validBits = 0;
    int numChannels = 0;
    int numFrames = 0;
    bool isFloat = false;

    REQUIRE(wav_read_header(stream, sampleRate, validBits, numChannels,
                            numFrames, isFloat));
    REQUIRE_FALSE(isFloat);

    stream->clear();
    stream->seekg(0, std::ios::beg);

    SoundPlayer soundPlayer;
    REQUIRE(soundPlayer.start(stream, SoundPlayerFileFormat::WAV, 44100));

    AudioBuffer buffer("preview", 2, 3, 44100);

    for (int i = 0; i < 20; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        soundPlayer.processAudio(&buffer, 3);
        if (!buffer.isSilent())
        {
            break;
        }
    }

    REQUIRE(std::fabs(buffer.getChannel(0)[0] - 0.5f) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[1] + 0.5f) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[2]) < 0.0001f);
}

TEST_CASE("MPC60 SND preview decodes packed 12-bit samples", "[snd][preview]")
{
    const auto bytes =
        resourceBytes("test/RealMpc60/Snd/Mpc60V214Rock/HAT2CLSD.SND");
    auto stream = std::make_shared<std::stringstream>(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary);

    SoundPlayer soundPlayer;
    REQUIRE(soundPlayer.start(stream, SoundPlayerFileFormat::SND, 40000));

    AudioBuffer buffer("preview", 2, 4, 40000);

    for (int i = 0; i < 20; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        soundPlayer.processAudio(&buffer, 4);
        if (!buffer.isSilent())
        {
            break;
        }
    }

    REQUIRE(std::fabs(buffer.getChannel(0)[0] - short_to_float(27)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[1] - short_to_float(52)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[2] - short_to_float(55)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[3] - short_to_float(69)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(1)[0] - buffer.getChannel(0)[0]) <
            0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(1)[3] - buffer.getChannel(0)[3]) <
            0.0001f);
}

TEST_CASE("MPC3000 SND preview streams mono 16-bit PCM", "[snd][preview]")
{
    const auto bytes = resourceBytes("test/RealMpc3000/Snd/SOUND017.SND");
    auto stream = std::make_shared<std::stringstream>(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary);

    SoundPlayer soundPlayer;
    REQUIRE(soundPlayer.start(stream, SoundPlayerFileFormat::SND, 44100));

    AudioBuffer buffer("preview", 2, 4, 44100);

    for (int i = 0; i < 20; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        soundPlayer.processAudio(&buffer, 4);
        if (!buffer.isSilent())
        {
            break;
        }
    }

    REQUIRE(std::fabs(buffer.getChannel(0)[0] - short_to_float(5)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[1] - short_to_float(6)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[2] - short_to_float(7)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(0)[3] - short_to_float(6)) < 0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(1)[0] - buffer.getChannel(0)[0]) <
            0.0001f);
    REQUIRE(std::fabs(buffer.getChannel(1)[3] - buffer.getChannel(0)[3]) <
            0.0001f);
}
