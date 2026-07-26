#include <catch2/catch_test_macros.hpp>

#include "IntTypes.hpp"
#include "engine/Voice.hpp"
#include "engine/audio/core/AudioBuffer.hpp"
#include "performance/Drum.hpp"
#include "sampler/Sound.hpp"

#include <cmath>
#include <memory>
#include <vector>

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::engine::audio::core;
using namespace mpc::performance;
using namespace mpc::sampler;

namespace
{
    std::shared_ptr<Sound> makeConstantSound(const int frames, const int level)
    {
        auto result = std::make_shared<Sound>(44100);
        result->setSampleData(
            std::make_shared<std::vector<float>>(frames, 1.0f));
        result->setMono(true);
        result->setLevel(level);
        return result;
    }

    void initializeVoice(Voice &voice, const std::shared_ptr<Sound> &sound)
    {
        NoteParameters parameters;
        parameters.noteNumber = DrumNoteNumber(35);
        parameters.soundIndex = 0;
        parameters.attack = 0;
        parameters.decay = 100;
        parameters.decayMode = 0;

        voice.init(127, sound, 35, parameters, 0, 64, 0, 0, true, 0, 44100,
                   1, ProgramIndex(0));
    }
} // namespace

TEST_CASE("Voice uses MPC2000XL envelope target for sound level",
          "[amp-envelope][voice]")
{
    AudioBuffer buffer("voice", 2, 64, 44100);

    Voice silentVoice(0, false);
    initializeVoice(silentVoice, makeConstantSound(200000, 0));
    silentVoice.processAudio(&buffer, 64);
    REQUIRE(silentVoice.isFinished());
    REQUIRE(buffer.isSilent());

    Voice audibleVoice(0, false);
    initializeVoice(audibleVoice, makeConstantSound(200000, 100));
    audibleVoice.processAudio(&buffer, 64);
    REQUIRE_FALSE(audibleVoice.isFinished());
    REQUIRE_FALSE(buffer.isSilent());
}

TEST_CASE("Voice preserves static anti-click envelope on early release",
          "[amp-envelope][voice]")
{
    Voice voice(0, false);
    initializeVoice(voice, makeConstantSound(200000, 100));

    AudioBuffer initial("initial", 2, 1024, 44100);
    voice.processAudio(&initial, 1024);
    const auto levelBeforeRelease = initial.getChannel(0).back();

    voice.startDecay();

    AudioBuffer release("release", 2, 2, 44100);
    voice.processAudio(&release, 2);
    const auto &released = release.getChannel(0);

    REQUIRE(std::abs(released[0] - levelBeforeRelease) < 0.01f);
    REQUIRE(std::abs(released[1] - released[0]) < 0.01f);
    REQUIRE(released[1] < released[0]);
}
