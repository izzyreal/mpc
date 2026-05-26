#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "lcdgui/ScreenComponent.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

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

    constexpr auto pressErase = mpc::client::event::ClientHardwareEvent{
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

    constexpr auto releaseErase = mpc::client::event::ClientHardwareEvent{
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

TEST_CASE("ERASE erases all events",
          "[erase-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);

    stateManager->drainQueue();

    mpc::sequencer::EventData finalizedNote;
    finalizedNote.type = mpc::sequencer::EventType::NoteOn;
    finalizedNote.noteNumber = mpc::MinDrumNoteNumber;
    finalizedNote.duration = mpc::MinDuration;

    finalizedNote.tick = mpc::Tick{0};
    seq->getTrack(0)->acquireAndInsertEvent(finalizedNote);

    finalizedNote.tick = mpc::Tick{96};
    seq->getTrack(0)->acquireAndInsertEvent(finalizedNote);

    finalizedNote.tick = mpc::Tick{192};
    seq->getTrack(0)->acquireAndInsertEvent(finalizedNote);

    finalizedNote.tick = mpc::Tick{288};
    seq->getTrack(0)->acquireAndInsertEvent(finalizedNote);

    stateManager->drainQueue();

    assert(seq->getTrack(0)->getNoteEvents().size() == 4);

    mpc.getLayeredScreen()->openScreenById(mpc::lcdgui::ScreenId::SequencerScreen);

    const auto hwController =
    mpc.clientEventController->clientHardwareEventController;

    constexpr auto pressErase = mpc::client::event::ClientHardwareEvent{
        mpc::client::event::ClientHardwareEvent::Source::HostInputGesture,
        mpc::client::event::ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        mpc::hardware::ComponentId::ERASE,
        1.f,
        std::nullopt,
        std::nullopt};

    hwController->handleClientHardwareEvent(pressErase);

    constexpr auto releaseErase = mpc::client::event::ClientHardwareEvent{
        mpc::client::event::ClientHardwareEvent::Source::HostInputGesture,
        mpc::client::event::ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        mpc::hardware::ComponentId::ERASE,
        0.f,
        std::nullopt,
        std::nullopt};

    hwController->handleClientHardwareEvent(releaseErase);

    assert(mpc.getLayeredScreen()->getCurrentScreenId() ==
            mpc::lcdgui::ScreenId::EraseScreen);

    constexpr auto pressF5 = mpc::client::event::ClientHardwareEvent{
        mpc::client::event::ClientHardwareEvent::Source::HostInputGesture,
        mpc::client::event::ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        mpc::hardware::ComponentId::F5,
        1.f,
        std::nullopt,
        std::nullopt};

    hwController->handleClientHardwareEvent(pressF5);

    constexpr auto releaseF5 = mpc::client::event::ClientHardwareEvent{
        mpc::client::event::ClientHardwareEvent::Source::HostInputGesture,
        mpc::client::event::ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        mpc::hardware::ComponentId::F5,
        0.f,
        std::nullopt,
        std::nullopt};

    hwController->handleClientHardwareEvent(releaseF5);

    assert(mpc.getLayeredScreen()->getCurrentScreenId() ==
        mpc::lcdgui::ScreenId::SequencerScreen);

    stateManager->drainQueue();

    REQUIRE(seq->getTrack(0)->getNoteEvents().size() == 0);
}
