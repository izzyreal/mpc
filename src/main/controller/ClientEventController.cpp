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

