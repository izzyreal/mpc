#include <catch2/catch_test_macros.hpp>

#include "SampleOps.hpp"
#include "file/kaitai/generated/mpc2000snd.h"
#include "file/sndreader/SndReader.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "sampler/Sound.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

CMRC_DECLARE(mpctest);

using namespace mpc::file::sndreader;
using namespace mpc::file::sndwriter;
using namespace mpc::sampleops;

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

    std::stringstream parseStream(
        std::string(handwrittenBytes.begin(), handwrittenBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000snd_t parsed(&parseIo);
    parsed._read();

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
    REQUIRE(parsed.frames() != nullptr);
    REQUIRE(parsed.frames()->size() == 2U);
    REQUIRE(parsed.frames()->at(0) == 1234);
    REQUIRE(parsed.frames()->at(1) == -1234);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
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

TEST_CASE("Kaitai MPC2000 SND parses real mono and stereo files", "[kaitai-snd][hardware-files]")
{
    auto parseFile = [](const std::string& resourcePath, auto&& assertions) {
        auto fs = cmrc::mpctest::get_filesystem();
        auto file = fs.open(resourcePath);
        std::stringstream stream(
            std::string(
                std::string_view(file.begin(), file.end() - file.begin())
            ),
            std::ios::in | std::ios::out | std::ios::binary
        );
        kaitai::kstream ks(&stream);
        mpc2000snd_t parsed(&ks);
        parsed._read();
        assertions(parsed);
    };

    parseFile("test/KaitaiSnd/mpc2000snd_mono_loop_off.SND", [](const mpc2000snd_t& mono) {
        REQUIRE(mono.magic() == std::string("\x01\x04", 2));
        REQUIRE(mono.level() == 100);
        REQUIRE(mono.tune() == 0);
        REQUIRE(mono.start() == 0U);
        REQUIRE(mono.end() == 8012U);
        REQUIRE(mono.frame_count() == 8012U);
        REQUIRE(mono.loop_frame_count() == 8012U);
        REQUIRE(mono.beat_count() == 4);
        REQUIRE(mono.sample_rate() == 44100);
        REQUIRE(mono.stereo() == false);
    });

    parseFile("test/KaitaiSnd/mpc2000snd_stereo_loop_off.SND", [](const mpc2000snd_t& stereo) {
        REQUIRE(stereo.magic() == std::string("\x01\x04", 2));
        REQUIRE(stereo.level() == 100);
        REQUIRE(stereo.tune() == 0);
        REQUIRE(stereo.start() == 0U);
        REQUIRE(stereo.end() == 8012U);
        REQUIRE(stereo.frame_count() == 8012U);
        REQUIRE(stereo.loop_frame_count() == 8012U);
        REQUIRE(stereo.beat_count() == 4);
        REQUIRE(stereo.sample_rate() == 44100);
        REQUIRE(stereo.stereo() == true);
    });
}
