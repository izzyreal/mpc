#pragma once

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include <variant>

using namespace mpc::controller;
using namespace mpc::client::event;

ClientEventController::ClientEventController(mpc::Mpc &mpc)
    : keyboardBindings(std::make_shared<mpc::input::KeyboardBindings>())
{
    clientHardwareEventController = std::make_shared<ClientHardwareEventController>(mpc);
    clientMidiEventController = std::make_shared<ClientMidiEventController>();
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
