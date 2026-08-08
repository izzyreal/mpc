#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "TestMpc.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "engine/EngineHost.hpp"
#include "engine/PhysicalInteractionSoundPlayer.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/AudioMixerBus.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "hardware/ComponentId.hpp"

#include <algorithm>
#include <array>
#include <cmath>

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::engine;
using namespace mpc::hardware;

namespace
{
    constexpr int BufferSize = 512;

    ClientHardwareEvent buttonEvent(const ComponentId componentId,
                                    const ClientHardwareEvent::Type type)
    {
        ClientHardwareEvent event;
        event.source = ClientHardwareEvent::Source::Internal;
        event.type = type;
        event.componentId = componentId;
        return event;
    }

    void prepareAudio(Mpc &mpc)
    {
        const auto server = mpc.getEngineHost()->getAudioServer();
        server->setSampleRate(48000);
        server->resizeBuffers(BufferSize);
    }
} // namespace

TEST_CASE("All first-iteration physical button sounds are bundled",
          "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    REQUIRE(mpc.getEngineHost()
                ->getPhysicalInteractionSoundPlayer()
                ->getLoadedSampleCount() == 184);
}

TEST_CASE("Button press and release transitions render to stereo out",
          "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareAudio(mpc);

    const auto engineHost = mpc.getEngineHost();
    engineHost->setPhysicalSoundsMixMode(PhysicalSoundsMixMode::StereoOut);

    const auto controller =
        mpc.clientEventController->clientHardwareEventController;
    controller->handleClientHardwareEvent(
        buttonEvent(F1, ClientHardwareEvent::Type::MpcButtonPress));

    engineHost->prepareProcessBlock(BufferSize);
    engineHost->getMixer()->work(BufferSize);

    REQUIRE_FALSE(
        engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());

    engineHost->setPhysicalSoundsEnabled(false);
    engineHost->prepareProcessBlock(BufferSize);
    engineHost->getMixer()->work(BufferSize);
    REQUIRE(engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());

    engineHost->setPhysicalSoundsEnabled(true);
    controller->handleClientHardwareEvent(
        buttonEvent(F1, ClientHardwareEvent::Type::MpcButtonRelease));
    engineHost->prepareProcessBlock(BufferSize);
    engineHost->getMixer()->work(BufferSize);

    REQUIRE_FALSE(
        engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());
}

TEST_CASE("Dedicated physical-sounds mode reaches its own stereo output only",
          "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareAudio(mpc);

    const auto engineHost = mpc.getEngineHost();
    engineHost->setPhysicalSoundsMixMode(PhysicalSoundsMixMode::Dedicated);

    const auto controller =
        mpc.clientEventController->clientHardwareEventController;
    controller->handleClientHardwareEvent(
        buttonEvent(F2, ClientHardwareEvent::Type::MpcButtonPress));
    engineHost->prepareProcessBlock(BufferSize);

    std::array<float, BufferSize> left{};
    std::array<float, BufferSize> right{};
    std::array<float *, 2> output{left.data(), right.data()};
    engineHost->getAudioServer()->work(nullptr, output.data(), BufferSize, {},
                                       {10, 11}, {}, {0, 1});

    const auto hasPhysicalSound = [](const auto &channel)
    {
        return std::any_of(channel.begin(), channel.end(),
                           [](const float sample)
                           {
                               return std::abs(sample) > 0.f;
                           });
    };

    REQUIRE(hasPhysicalSound(left));
    REQUIRE(hasPhysicalSound(right));
    REQUIRE(engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());
}
