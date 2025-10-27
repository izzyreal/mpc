#include "controller/ClientEventController.hpp"

#include "controller/ClientHardwareEventController.hpp"
#include "controller/ClientMidiEventController.hpp"

#include "hardware/Hardware.hpp"

#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"

#include "hardware/ComponentId.hpp"

#include "sequencer/SeqUtil.hpp"

#include <variant>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::sequencer;
using namespace mpc::hardware;
using namespace mpc::lcdgui;

ClientEventController::ClientEventController(mpc::Mpc &mpcToUse)
    : keyboardBindings(std::make_shared<mpc::input::KeyboardBindings>()),
      mpc(mpcToUse),
      sequencer(mpc.getSequencer()),
      screens(mpc.screens),
      layeredScreen(mpc.getLayeredScreen()),
      hardware(mpc.getHardware())
{
}

void ClientEventController::init()
{
    clientHardwareEventController = std::make_shared<ClientHardwareEventController>(mpc);

    clientMidiEventController = std::make_shared<ClientMidiEventController>(
        shared_from_this(),
        clientHardwareEventController,
        screens->get<MidiSwScreen>(),
        sequencer,
        screens->get<MidiInputScreen>(),
        mpc.getEventHandler(),
        screens->get<MultiRecordingSetupScreen>(),
        screens->get<TimingCorrectScreen>());
}

void ClientEventController::dispatchHostInput(const mpc::input::HostInputEvent &hostEvent)
{
    const auto clientEvent = mpc::input::HostToClientTranslator::translate(hostEvent, keyboardBindings);

    if (!clientEvent.has_value())
    {
        printf("empty clientEvent\n");
        return;
    }

    handleClientEvent(*clientEvent);
}

void ClientEventController::handleClientEvent(const client::event::ClientEvent &e)
{
    if (std::holds_alternative<ClientHardwareEvent>(e.payload))
    {
        clientHardwareEventController->handleClientHardwareEvent(std::get<ClientHardwareEvent>(e.payload));
    }
    else if (std::holds_alternative<ClientMidiEvent>(e.payload))
    {
        clientMidiEventController->handleClientMidiEvent(std::get<ClientMidiEvent>(e.payload));
    }
}

std::shared_ptr<ClientMidiEventController> ClientEventController::getClientMidiEventController()
{
    return clientMidiEventController;
}

RecordingMode ClientEventController::determineRecordingMode() const
{
    if (sequencer->isRecordingOrOverdubbing())
    {
        return RecordingMode::Overdub;
    }
    if (SeqUtil::isStepRecording(layeredScreen->getCurrentScreenName(), sequencer))
    {
        return RecordingMode::Step;
    }

    if (SeqUtil::isRecMainWithoutPlaying(
            sequencer,
            screens->get<TimingCorrectScreen>(),
            layeredScreen->getCurrentScreenName(),
            hardware->getButton(ComponentId::REC),
            clientHardwareEventController))
    {
        return RecordingMode::RecMainWithoutPlaying;
    }

    return RecordingMode::None;
}

std::shared_ptr<LayeredScreen> ClientEventController::getLayeredScreen()
{
    return layeredScreen;
}

void ClientEventController::setSelectedPad(int padIndexWithBank)
{
    selectedPad = std::clamp(padIndexWithBank, 0, 63);

    notifyObservers(std::string("pad"));
}

void ClientEventController::setActiveBank(const Bank activeBankToUse)
{
    activeBank = activeBankToUse;

    notifyObservers(std::string("bank"));

    hardware->getLed(hardware::ComponentId::BANK_A_LED)->setEnabled(activeBank == Bank::A);
    hardware->getLed(hardware::ComponentId::BANK_B_LED)->setEnabled(activeBank == Bank::B);
    hardware->getLed(hardware::ComponentId::BANK_C_LED)->setEnabled(activeBank == Bank::C);
    hardware->getLed(hardware::ComponentId::BANK_D_LED)->setEnabled(activeBank == Bank::D);
}

Bank ClientEventController::getActiveBank() const
{
    return activeBank;
}

void ClientEventController::setSelectedNote(const int selectedNoteToUse)
{
    selectedNote = std::clamp(selectedNoteToUse, 35, 98);
    notifyObservers(std::string("note"));
}

int ClientEventController::getSelectedNote() const
{
    return selectedNote;
}

int ClientEventController::getSelectedPad() const
{
    return selectedPad;
}

bool ClientEventController::isAfterEnabled() const
{
    return afterEnabled;
}

void ClientEventController::setAfterEnabled(bool b)
{
    afterEnabled = b;
}

bool ClientEventController::isFullLevelEnabled() const
{
    return fullLevelEnabled;
}

void ClientEventController::setFullLevelEnabled(bool b)
{
    fullLevelEnabled = b;
}

bool ClientEventController::isSixteenLevelsEnabled() const
{
    return sixteenLevelsEnabled;
}

void ClientEventController::setSixteenLevelsEnabled(bool b)
{
    sixteenLevelsEnabled = b;
}
