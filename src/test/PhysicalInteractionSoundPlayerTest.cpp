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
#include "performance/PerformanceManager.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"

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

    ClientHardwareEvent padEvent(const ClientHardwareEvent::Type type,
                                 const float velocity = 1.f)
    {
        ClientHardwareEvent event;
        event.source = ClientHardwareEvent::Source::Internal;
        event.type = type;
        event.componentId = PAD_1_OR_AB;
        event.index = 0;
        event.value = velocity;
        return event;
    }

    void prepareAudio(Mpc &mpc)
    {
        const auto server = mpc.getEngineHost()->getAudioServer();
        server->setSampleRate(48000);
        server->resizeBuffers(BufferSize);
    }

    void preparePadInput(Mpc &mpc)
    {
        mpc.getSampler()->getProgram(0)->setUsed();
        mpc.getSampler()->getProgram(0)->initPadAssign();
        mpc.getSequencer()->getSelectedSequence()->init(1);
        mpc.getSequencer()->getStateManager()->drainQueue();
        mpc.getPerformanceManager().lock()->drainQueue();
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

TEST_CASE("All physical interaction sounds are bundled", "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    const auto player =
        mpc.getEngineHost()->getPhysicalInteractionSoundPlayer();
    REQUIRE(player->getLoadedSampleCount() == 234);
    REQUIRE(player->getLoadedPadSampleCount() == 48);
    REQUIRE(player->getLoadedPowerSampleCount() == 2);
}

TEST_CASE("Accepted pad presses render velocity-layered physical sounds",
          "[physical-sounds]")
{
    const auto renderPad = [](const float velocity)
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        preparePadInput(mpc);
        prepareAudio(mpc);

        const auto engineHost = mpc.getEngineHost();
        engineHost->setPhysicalSoundsMixMode(
            PhysicalSoundsMixMode::StereoOut);
        mpc.clientEventController->clientHardwareEventController
            ->handleClientHardwareEvent(
                padEvent(ClientHardwareEvent::Type::PadPress, velocity));
        float peak = 0.f;
        for (int i = 0; i < 20; ++i)
        {
            engineHost->prepareProcessBlock(BufferSize);
            const auto output = renderOutputs(mpc);
            for (const auto sample : output.stereoLeft)
            {
                peak = std::max(peak, std::abs(sample));
            }
        }
        return peak;
    };

    const auto soft = renderPad(0.f);
    const auto hard = renderPad(1.f);
    REQUIRE(soft > 0.f);
    REQUIRE(hard > soft);
}

TEST_CASE("A held pad does not retrigger its physical sound",
          "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    preparePadInput(mpc);
    prepareAudio(mpc);

    const auto engineHost = mpc.getEngineHost();
    const auto controller =
        mpc.clientEventController->clientHardwareEventController;
    controller->handleClientHardwareEvent(
        padEvent(ClientHardwareEvent::Type::PadPress, 0.5f));

    for (int i = 0; i < 24; ++i)
    {
        engineHost->prepareProcessBlock(BufferSize);
        renderOutputs(mpc);
    }

    controller->handleClientHardwareEvent(
        padEvent(ClientHardwareEvent::Type::PadPress, 0.5f));
    engineHost->prepareProcessBlock(BufferSize);
    const auto output = renderOutputs(mpc);
    REQUIRE_FALSE(hasSound(output.stereoLeft));
    REQUIRE_FALSE(hasSound(output.physicalLeft));
}

TEST_CASE("Power-off lifecycle voice renders completely", "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareAudio(mpc);

    const auto engineHost = mpc.getEngineHost();
    REQUIRE(engineHost->beginPhysicalPowerOffSound());
    const auto expectedBlockCount = static_cast<int>(std::ceil(
        engineHost->getPhysicalPowerOffSoundDurationSeconds() * 48000.0 /
        static_cast<double>(BufferSize)));

    bool heardSound = false;
    int renderedBlocks = 0;
    while (!engineHost->isPhysicalPowerOffSoundComplete() &&
           renderedBlocks < expectedBlockCount + 2)
    {
        engineHost->prepareProcessBlock(BufferSize);
        const auto output = renderOutputs(mpc);
        heardSound = heardSound || hasSound(output.stereoLeft);
        ++renderedBlocks;
    }

    REQUIRE(heardSound);
    REQUIRE(engineHost->isPhysicalPowerOffSoundComplete());
    REQUIRE(renderedBlocks == expectedBlockCount);
}

TEST_CASE("Muted physical sounds do not delay power-off",
          "[physical-sounds]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    const auto engineHost = mpc.getEngineHost();

    engineHost->setPhysicalSoundsEnabled(false);
    REQUIRE_FALSE(engineHost->beginPhysicalPowerOffSound());

    engineHost->setPhysicalSoundsEnabled(true);
    engineHost->setPhysicalSoundsLevel(0);
    REQUIRE_FALSE(engineHost->beginPhysicalPowerOffSound());
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
        engineHost->setPhysicalSoundsMixMode(PhysicalSoundsMixMode::StereoOut);
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
