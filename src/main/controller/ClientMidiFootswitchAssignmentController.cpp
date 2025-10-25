#include "controller/ClientMidiFootswitchAssignmentController.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include "hardware/Component.hpp"

#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens;
using namespace mpc::hardware;

ClientMidiFootswitchAssignmentController::ClientMidiFootswitchAssignmentController(
        std::shared_ptr<ClientHardwareEventController> clientHardwareEventControllerToUse,
        std::shared_ptr<MidiSwScreen> midiSwScreenToUse)
    : clientHardwareEventController(clientHardwareEventControllerToUse),
    midiSwScreen(midiSwScreenToUse)
{
}

void ClientMidiFootswitchAssignmentController::handleEvent(const ClientMidiEvent &e)
{
    std::cout << "[FootswitchAssignment] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;

    if (e.getMessageType() != ClientMidiEvent::MessageType::CONTROLLER)
    {
        return;
    }

    const int controller = e.getControllerNumber();
    const int value = e.getControllerValue();

    const bool pressed = value >= 64;

    std::cout << "\n[FootswitchAssignment] Handling controller " << controller
              << " (val=" << value << (pressed ? ", pressed" : ", released") << ")\n";

    for (auto [switchCC, functionIndex] : midiSwScreen->getSwitches())
    {
        if (switchCC != controller) continue;

        const std::string &fn = midiSwScreen->getFunctionNames()[functionIndex];

        ClientHardwareEvent ev{};
        ev.source = ClientHardwareEvent::Source::HostInputMidi;

        if (fn == "PLAY STRT")
        {
            ev.componentId = ComponentId::PLAY_START;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "PLAY")
        {
            ev.componentId = ComponentId::PLAY;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "STOP")
        {
            ev.componentId = ComponentId::STOP;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "TAP")
        {
            ev.componentId = ComponentId::TAP_TEMPO_OR_NOTE_REPEAT;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "PAD BNK A")
        {
            ev.componentId = ComponentId::BANK_A;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "PAD BNK B")
        {
            ev.componentId = ComponentId::BANK_B;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "PAD BNK C")
        {
            ev.componentId = ComponentId::BANK_C;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "PAD BNK D")
        {
            ev.componentId = ComponentId::BANK_D;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn.rfind("PAD", 0) == 0) // " PAD  1" .. " PAD 16"
        {
            const int padNum = std::stoi(fn.substr(6)); // extract 1–16
            ev.componentId = static_cast<ComponentId>(static_cast<int>(ComponentId::PAD_1_OR_AB) + padNum - 1);
            ev.type = pressed ? ClientHardwareEvent::Type::PadPress : ClientHardwareEvent::Type::PadRelease;
            ev.index = padNum - 1;

            if (pressed)
            {
                ev.value = mpc::hardware::Pad::MAX_VELO;
            }
        }
        else if (fn.rfind("F", 0) == 0 || fn.find("   F") != std::string::npos)
        {
            // "   F1".."   F6"
            const int fNum = std::stoi(fn.substr(fn.find('F') + 1));
            ev.componentId = static_cast<ComponentId>(static_cast<int>(ComponentId::F1) + fNum - 1);
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
        }
        else if (fn == "REC+PLAY" || fn == "ODUB+PLAY" || fn == "REC/PUNCH" || fn == "ODUB/PNCH")
        {
            std::cout << "[FootswitchAssignment] Stub for multi-button combo: " << fn << std::endl;
        }

        clientHardwareEventController->handleClientHardwareEvent(ev);
    }
}

