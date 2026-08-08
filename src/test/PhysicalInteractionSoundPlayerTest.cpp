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

    struct RenderedOutputs
    {
        std::array<float, BufferSize> stereoLeft{};
        std::array<float, BufferSize> stereoRight{};
        std::array<float, BufferSize> physicalLeft{};
        std::array<float, BufferSize> physicalRight{};
    };

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

    RenderedOutputs renderOutputs(Mpc &mpc)
    {
        RenderedOutputs result;
        std::array<float *, 4> output{result.stereoLeft.data(),
                                     result.stereoRight.data(),
                                     result.physicalLeft.data(),
                                     result.physicalRight.data()};
        mpc.getEngineHost()->getAudioServer()->work(
            nullptr, output.data(), BufferSize, {}, {0, 1, 10, 11}, {},
            {0, 1, 2, 3});
        return result;
    }

    template <typename Channel> bool hasSound(const Channel &channel)
    {
        return std::any_of(channel.begin(), channel.end(),
                           [](const float sample)
                           {
                               return std::abs(sample) > 0.f;
                           });
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
    const auto pressOutput = renderOutputs(mpc);

    REQUIRE(hasSound(pressOutput.stereoLeft));
    REQUIRE(hasSound(pressOutput.stereoRight));
    REQUIRE_FALSE(hasSound(pressOutput.physicalLeft));
    REQUIRE_FALSE(hasSound(pressOutput.physicalRight));
    REQUIRE(engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());

    engineHost->setPhysicalSoundsEnabled(false);
    engineHost->prepareProcessBlock(BufferSize);
    const auto disabledOutput = renderOutputs(mpc);
    REQUIRE_FALSE(hasSound(disabledOutput.stereoLeft));
    REQUIRE_FALSE(hasSound(disabledOutput.stereoRight));

    engineHost->setPhysicalSoundsEnabled(true);
    controller->handleClientHardwareEvent(
        buttonEvent(F1, ClientHardwareEvent::Type::MpcButtonRelease));
    engineHost->prepareProcessBlock(BufferSize);
    const auto releaseOutput = renderOutputs(mpc);

    REQUIRE(hasSound(releaseOutput.stereoLeft));
    REQUIRE(hasSound(releaseOutput.stereoRight));
    REQUIRE_FALSE(hasSound(releaseOutput.physicalLeft));
    REQUIRE_FALSE(hasSound(releaseOutput.physicalRight));
    REQUIRE(engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());
}

TEST_CASE("MAIN VOLUME does not affect physical sounds on stereo out",
          "[physical-sounds]")
{
    const auto renderAtMainLevel = [](const int mainLevel)
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        prepareAudio(mpc);

        const auto engineHost = mpc.getEngineHost();
        engineHost->setMainLevel(mainLevel);
        engineHost->setPhysicalSoundsMixMode(
            PhysicalSoundsMixMode::StereoOut);
        mpc.clientEventController->clientHardwareEventController
            ->handleClientHardwareEvent(
                buttonEvent(F3, ClientHardwareEvent::Type::MpcButtonPress));
        engineHost->prepareProcessBlock(BufferSize);
        return renderOutputs(mpc);
    };

    const auto mutedMainOutput = renderAtMainLevel(0);
    const auto fullMainOutput = renderAtMainLevel(100);

    REQUIRE(hasSound(mutedMainOutput.stereoLeft));
    REQUIRE(mutedMainOutput.stereoLeft == fullMainOutput.stereoLeft);
    REQUIRE(mutedMainOutput.stereoRight == fullMainOutput.stereoRight);
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

    const auto output = renderOutputs(mpc);

    REQUIRE_FALSE(hasSound(output.stereoLeft));
    REQUIRE_FALSE(hasSound(output.stereoRight));
    REQUIRE(hasSound(output.physicalLeft));
    REQUIRE(hasSound(output.physicalRight));
    REQUIRE(engineHost->getMixer()->getMainBus()->getBuffer()->isSilent());
}
