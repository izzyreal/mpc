#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "TestMpc.hpp"
#include "engine/BasicSoundPlayer.hpp"
#include "engine/EngineHost.hpp"
#include "engine/MixerConstants.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::sampler;

namespace
{
    constexpr int kBufferSize = 64;

    void addPreviewSound(Mpc &mpc)
    {
        const auto sound = mpc.getSampler()->addSound();
        REQUIRE(sound != nullptr);

        sound->setMono(true);
        sound->insertFrame(std::vector<float>{1.f}, 0);
        sound->setStart(0);
        sound->setEnd(1);
    }
} // namespace

TEST_CASE("Preview and metronome players keep distinct mixer strip bindings",
          "[basic-sound-player]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    addPreviewSound(mpc);

    const auto engineHost = mpc.getEngineHost();
    const auto mixer = engineHost->getMixer();

    engineHost->getPreviewSoundPlayer()->playSound(PREVIEW_SOUND, 127, 0);
    REQUIRE(
        mixer->getStrip(PreviewSoundPlayerStrip)->processBuffer(kBufferSize));

    engineHost->getMetronomePlayer()->playSound(CLICK_SOUND, 127, 0);
    REQUIRE(mixer->getStrip(MetronomeStrip)->processBuffer(kBufferSize));
}
