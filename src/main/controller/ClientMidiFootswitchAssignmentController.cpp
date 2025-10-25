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
        std::shared_ptr<MidiSwScreen> midiSwScreenToUse,
        std::shared_ptr<sequencer::Sequencer> sequencerToUse)
    : clientHardwareEventController(clientHardwareEventControllerToUse),
    midiSwScreen(midiSwScreenToUse),
    sequencer(sequencerToUse)
{
}

void ClientMidiFootswitchAssignmentController::pressButton(hardware::ComponentId componentId)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::HostInputMidi;
    ev.componentId = componentId;
    ev.type = ClientHardwareEvent::Type::ButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButton(hardware::ComponentId componentId)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::HostInputMidi;
    ev.componentId = componentId;
    ev.type = ClientHardwareEvent::Type::ButtonRelease;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::triggerDualButtonCombo(ComponentId first, ComponentId second)
{
    pressButton(first);
    pressButton(second);
    releaseButton(second);
    releaseButton(first);
}

void ClientMidiFootswitchAssignmentController::handleStopToPlay()
{
    pressButton(ComponentId::PLAY);
    releaseButton(ComponentId::PLAY);
}

void ClientMidiFootswitchAssignmentController::handleRecordingToPlay()
{
    pressButton(ComponentId::REC);
    releaseButton(ComponentId::REC);
}

void ClientMidiFootswitchAssignmentController::handleRecPunch()
{
    std::cout << "[FootswitchAssignment] Handling REC/PUNCH" << std::endl;
    
    if (!sequencer->isPlaying())
    {
        // STOP mode -> PLAY mode
        handleStopToPlay();
    }
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing())
    {
        // PLAY mode -> REC (Punch-in) mode
        pressButton(ComponentId::REC);
        pressButton(ComponentId::PLAY);
        releaseButton(ComponentId::PLAY);
        releaseButton(ComponentId::REC);
    }
    else if (sequencer->isRecording())
    {
        // REC mode -> PLAY (Punch-out) mode
        handleRecordingToPlay();
    }
}

void ClientMidiFootswitchAssignmentController::handleOdubPunch()
{
    std::cout << "[FootswitchAssignment] Handling ODUB/PNCH" << std::endl;
    
    if (!sequencer->isPlaying())
    {
        // STOP mode -> PLAY mode
        handleStopToPlay();
    }
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing())
    {
        // PLAY mode -> OVERDUB (Punch-in) mode
        pressButton(ComponentId::OVERDUB);
        pressButton(ComponentId::PLAY);
        releaseButton(ComponentId::PLAY);
        releaseButton(ComponentId::OVERDUB);
    }
    else if (sequencer->isOverdubbing())
    {
        // OVERDUB mode -> PLAY (Punch-out) mode
        handleRecordingToPlay();
    }
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

        if (fn.rfind("PAD", 0) == 0) // " PAD  1" .. " PAD 16"
        {
            ClientHardwareEvent ev;
            ev.source = ClientHardwareEvent::Source::HostInputMidi;
            const int padNum = std::stoi(fn.substr(6)); // extract 1–16
            ev.componentId = static_cast<ComponentId>(static_cast<int>(ComponentId::PAD_1_OR_AB) + padNum - 1);
            ev.index = padNum - 1;
            ev.type = pressed ? ClientHardwareEvent::Type::PadPress : ClientHardwareEvent::Type::PadRelease;

            if (pressed)
            {
                ev.value = mpc::hardware::Pad::MAX_VELO;
            }

            clientHardwareEventController->handleClientHardwareEvent(ev);
            continue;
        }

        std::optional<ComponentId> id;

        if (fn == "PLAY STRT")
        {
            id = ComponentId::PLAY_START;
        }
        else if (fn == "PLAY")
        {
            id = ComponentId::PLAY;
        }
        else if (fn == "STOP")
        {
            id = ComponentId::STOP;
        }
        else if (fn == "TAP")
        {
            id = ComponentId::TAP_TEMPO_OR_NOTE_REPEAT;
        }
        else if (fn == "PAD BNK A")
        {
            id = ComponentId::BANK_A;
        }
        else if (fn == "PAD BNK B")
        {
            id = ComponentId::BANK_B;
        }
        else if (fn == "PAD BNK C")
        {
            id = ComponentId::BANK_C;
        }
        else if (fn == "PAD BNK D")
        {
            id = ComponentId::BANK_D;
        }

        else if (fn.rfind("F", 0) == 0 || fn.find("   F") != std::string::npos)
        {
            // "   F1".."   F6"
            const int fNum = std::stoi(fn.substr(fn.find('F') + 1));
            id = static_cast<ComponentId>(static_cast<int>(ComponentId::F1) + fNum - 1);
        }

        if (id)
        {
            ClientHardwareEvent ev;
            ev.componentId = *id;
            ev.source = ClientHardwareEvent::Source::HostInputMidi;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress : ClientHardwareEvent::Type::ButtonRelease;
            clientHardwareEventController->handleClientHardwareEvent(ev);
            continue;
        }

        if (fn == "REC+PLAY")
        {
            if (pressed)
            {
                triggerDualButtonCombo(ComponentId::REC, ComponentId::PLAY);
            }

            continue;
        }
        else if (fn == "ODUB+PLAY")
        {
            if (pressed)
            {
                triggerDualButtonCombo(ComponentId::OVERDUB, ComponentId::PLAY);
            }

            continue;
        }
        else if (fn == "REC/PUNCH")
        {
            if (pressed)
            {
                handleRecPunch();
            }
        }
        else if (fn == "ODUB/PNCH")
        {
            if (pressed)
            {
                handleOdubPunch();
            }
        }
    }
}

