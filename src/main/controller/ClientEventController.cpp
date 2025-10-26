#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "hardware/ComponentId.hpp"

#include <variant>

using namespace mpc::controller;
using namespace mpc::client::event;

ClientEventController::ClientEventController(mpc::Mpc &mpc)
    : keyboardBindings(std::make_shared<mpc::input::KeyboardBindings>())
{
    clientHardwareEventController = std::make_shared<ClientHardwareEventController>(mpc);
    clientMidiEventController = std::make_shared<ClientMidiEventController>(
        clientHardwareEventController, 
        mpc.screens->get<MidiSwScreen>(), 
        mpc.getSequencer(),
        mpc.screens->get<MidiInputScreen>(),
        mpc.getEventHandler(),
        mpc.screens->get<MultiRecordingSetupScreen>(),
        mpc.screens->get<TimingCorrectScreen>(),
        mpc.getHardware()->getButton(hardware::ComponentId::REC),
        [layeredScreen = mpc.getLayeredScreen()] { return layeredScreen->getCurrentScreenName(); }
    );
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

