#include "controller/ClientEventController.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "controller/ClientMidiEventController.hpp"

#include "hardware/Hardware.hpp"

#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"

#include "hardware/ComponentId.hpp"

#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"

#include <variant>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::sequencer;
using namespace mpc::hardware;
using namespace mpc::lcdgui;

ClientEventController::ClientEventController(Mpc &mpcToUse)
    : setSelectedNote(
          [this](const DrumNoteNumber selectedNoteToUse)
          {
              selectedNote = std::clamp(selectedNoteToUse, MinDrumNoteNumber,
                                        MaxDrumNoteNumber);
              notifyObservers(std::string("note"));
          }),
      setSelectedPad(
          [this](const ProgramPadIndex programPadIndex)
          {
              selectedPad = std::clamp(programPadIndex, MinProgramPadIndex,
                                       MaxProgramPadIndex);
              notifyObservers(std::string("pad"));
          }),
      mpc(mpcToUse),
      keyboardBindings(std::make_shared<input::KeyboardBindings>()),
      screens(mpc.screens), layeredScreen(mpc.getLayeredScreen()),
      hardware(mpc.getHardware())
{
}

void ClientEventController::init()
{
    clientHardwareEventController =
        std::make_shared<ClientHardwareEventController>(mpc);

    clientMidiEventController = std::make_shared<ClientMidiEventController>(
        mpc.getPerformanceManager(), shared_from_this(),
        clientHardwareEventController, screens.lock()->get<ScreenId::MidiSwScreen>(),
        mpc.getSequencer(), mpc.getSampler(),
        screens.lock()->get<ScreenId::MidiInputScreen>(), mpc.getEventHandler(),
        screens.lock()->get<ScreenId::MultiRecordingSetupScreen>(), layeredScreen.lock(),
        hardware.lock(), screens.lock(), mpc.getEngineHost()->getPreviewSoundPlayer().get());
}

void ClientEventController::dispatchHostInput(
    const input::HostInputEvent &hostEvent)
{
    const auto clientEvent =
        hostToClientTranslator.translate(hostEvent, keyboardBindings);

    if (!clientEvent.has_value())
    {
        // printf("empty clientEvent\n");
        return;
    }

    handleClientEvent(*clientEvent);
}

void ClientEventController::handleClientEvent(const ClientEvent &e) const
{
    if (std::holds_alternative<ClientHardwareEvent>(e.payload))
    {
        clientHardwareEventController->handleClientHardwareEvent(
            std::get<ClientHardwareEvent>(e.payload));
    }
    else if (std::holds_alternative<ClientMidiEvent>(e.payload))
    {
        clientMidiEventController->handleClientMidiEvent(
            std::get<ClientMidiEvent>(e.payload));
    }
}

std::shared_ptr<ClientMidiEventController>
ClientEventController::getClientMidiEventController()
{
    return clientMidiEventController;
}

bool ClientEventController::isRecMainWithoutPlaying() const
{
    return SeqUtil::isRecMainWithoutPlaying(mpc.getSequencer(),
                                            layeredScreen.lock()->getCurrentScreenId(),
                                            clientHardwareEventController);
}

RecordingMode ClientEventController::determineRecordingMode() const
{
    if (mpc.getSequencer()->getTransport()->isRecordingOrOverdubbing())
    {
        return RecordingMode::Overdub;
    }
    if (SeqUtil::isStepRecording(layeredScreen.lock()->getCurrentScreenName(),
                                 mpc.getSequencer().get()))
    {
        return RecordingMode::Step;
    }

    if (isRecMainWithoutPlaying())
    {
        return RecordingMode::RecMainWithoutPlaying;
    }

    return RecordingMode::NoRecordingMode;
}

std::shared_ptr<LayeredScreen> ClientEventController::getLayeredScreen()
{
    return layeredScreen.lock();
}

void ClientEventController::setActiveBank(const Bank activeBankToUse)
{
    activeBank = activeBankToUse;

    notifyObservers(std::string("bank"));

    hardware.lock()->getLed(BANK_A_LED)->setEnabled(activeBank == Bank::A);
    hardware.lock()->getLed(BANK_B_LED)->setEnabled(activeBank == Bank::B);
    hardware.lock()->getLed(BANK_C_LED)->setEnabled(activeBank == Bank::C);
    hardware.lock()->getLed(BANK_D_LED)->setEnabled(activeBank == Bank::D);

    layeredScreen.lock()->postToUiThread(utils::Task(
        [this]
        {
            setActiveBankUiCallback(activeBank);
        }));
}

Bank ClientEventController::getActiveBank() const
{
    return activeBank;
}

mpc::DrumNoteNumber ClientEventController::getSelectedNote() const
{
    return selectedNote;
}

mpc::ProgramPadIndex ClientEventController::getSelectedPad() const
{
    return selectedPad;
}

bool ClientEventController::isAfterEnabled() const
{
    return afterEnabled;
}

void ClientEventController::setAfterEnabled(const bool b)
{
    afterEnabled = b;
}

bool ClientEventController::isFullLevelEnabled() const
{
    return fullLevelEnabled;
}

void ClientEventController::setFullLevelEnabled(const bool b)
{
    fullLevelEnabled = b;
}

bool ClientEventController::isSixteenLevelsEnabled() const
{
    return sixteenLevelsEnabled;
}

void ClientEventController::setSixteenLevelsEnabled(const bool b)
{
    sixteenLevelsEnabled = b;
}

bool ClientEventController::isEraseButtonPressed() const
{
    return hardware.lock()->getButton(ERASE)->isPressed();
}
