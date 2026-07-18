#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <vector>

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::lcdgui;

namespace
{
    ClientHardwareEvent makePadEvent(const ClientHardwareEvent::Type type)
    {
        ClientHardwareEvent e;
        e.source = ClientHardwareEvent::Source::HostInputGesture;
        e.type = type;
        e.componentId = hardware::PAD_1_OR_AB;
        e.index = 0;

        if (type == ClientHardwareEvent::Type::PadRelease)
        {
            e.value = 0.f;
        }
        else
        {
            e.value = 1.f;
        }

        return e;
    }

    void prepareMpcWithSound(Mpc &mpc)
    {
        TestMpc::initializeTestMpc(mpc);

        auto sound = mpc.getSampler()->addSound();
        sound->setName("test");
        sound->insertFrame(std::vector<float>{0.f}, 0);
        mpc.getSampler()->setSoundIndex(0);

        auto program = mpc.getSampler()->getProgram(0);
        program->setUsed();
        mpc.getSequencer()
            ->getDrumBus(DrumBusIndex(0))
            ->setProgramIndex(ProgramIndex(0));
        mpc.getSequencer()->getSelectedSequence()->init(1);
        mpc.getSequencer()->getStateManager()->drainQueue();
        mpc.getPerformanceManager().lock()->drainQueue();
    }

    int getActiveNoteCount(Mpc &mpc)
    {
        auto performanceManager = mpc.getPerformanceManager().lock();
        performanceManager->drainQueue();
        performanceManager->drainQueue();
        return performanceManager->getSnapshot().getTotalNoteOnCount();
    }

    void sendPadEvent(Mpc &mpc, const ClientHardwareEvent::Type type)
    {
        mpc.clientEventController->clientHardwareEventController
            ->handleClientHardwareEvent(makePadEvent(type));
    }
}

TEST_CASE("Shift+5 sampler screens suppress physical pad note generation",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::TrimScreen);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    sendPadEvent(mpc, ClientHardwareEvent::Type::PadAftertouch);
    REQUIRE(getActiveNoteCount(mpc) == 0);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);
}

TEST_CASE("Shift+5 sampler child screens suppress physical pad note generation",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::TrimScreen);
    layeredScreen->openScreenById(ScreenId::StartFineScreen);
    REQUIRE(layeredScreen->isCurrentScreenOrChildOf(ScreenId::TrimScreen));

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    REQUIRE(getActiveNoteCount(mpc) == 0);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);
}

TEST_CASE("Sequencer screen still generates notes for physical pad presses",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    REQUIRE(getActiveNoteCount(mpc) == 1);
}

TEST_CASE("Unused selected sequence suppresses physical pad note generation",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);

    auto sequencer = mpc.getSequencer();
    sequencer->setSelectedSequenceIndex(SequenceIndex(5), true);
    sequencer->getStateManager()->drainQueue();
    REQUIRE_FALSE(sequencer->getSelectedSequence()->isUsed());

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    sendPadEvent(mpc, ClientHardwareEvent::Type::PadAftertouch);
    REQUIRE(getActiveNoteCount(mpc) == 0);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);
}
