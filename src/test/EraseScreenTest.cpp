#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "lcdgui/ScreenComponent.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"

TEST_CASE("ERASE window keeps function keys visible after ERASE release",
          "[erase-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(mpc::lcdgui::ScreenId::SequencerScreen);

    const auto hwController =
        mpc.clientEventController->clientHardwareEventController;

    const auto pressErase = mpc::client::event::ClientHardwareEvent{
        mpc::client::event::ClientHardwareEvent::Source::HostInputGesture,
        mpc::client::event::ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        mpc::hardware::ComponentId::ERASE,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(pressErase);

    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenId() ==
            mpc::lcdgui::ScreenId::EraseScreen);
    REQUIRE_FALSE(mpc.getScreen()->findChild("function-keys")->IsHidden());

    const auto releaseErase = mpc::client::event::ClientHardwareEvent{
        mpc::client::event::ClientHardwareEvent::Source::HostInputGesture,
        mpc::client::event::ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        mpc::hardware::ComponentId::ERASE,
        0.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(releaseErase);

    REQUIRE(mpc.getLayeredScreen()->getCurrentScreenId() ==
            mpc::lcdgui::ScreenId::EraseScreen);
    REQUIRE_FALSE(mpc.getScreen()->findChild("function-keys")->IsHidden());
}
