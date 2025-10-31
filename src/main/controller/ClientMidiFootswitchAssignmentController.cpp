#include "controller/ClientMidiFootswitchAssignmentController.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include "hardware/Component.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens;
using namespace mpc::hardware;

ClientMidiFootswitchAssignmentController::
    ClientMidiFootswitchAssignmentController(
        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventControllerToUse,
        std::shared_ptr<MidiSwScreen> midiSwScreenToUse,
        std::shared_ptr<sequencer::Sequencer> sequencerToUse)
    : clientHardwareEventController(clientHardwareEventControllerToUse),
      midiSwScreen(midiSwScreenToUse), sequencer(sequencerToUse)
{
}

void ClientMidiFootswitchAssignmentController::pressButton(
    hardware::ComponentId componentId)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = componentId;
    ev.type = ClientHardwareEvent::Type::ButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButton(
    hardware::ComponentId componentId)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = componentId;
    ev.type = ClientHardwareEvent::Type::ButtonRelease;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::triggerDualButtonCombo(
    ComponentId first, ComponentId second)
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

void ClientMidiFootswitchAssignmentController::handleEvent(
    const ClientMidiEvent &e)
{
    if (e.getMessageType() != ClientMidiEvent::MessageType::CONTROLLER)
    {
        return;
    }

    const int controller = e.getControllerNumber();
    const int value = e.getControllerValue();

    const bool pressed = value >= 64;

    for (auto [switchCC, functionIndex] : midiSwScreen->getSwitches())
    {
        if (switchCC != controller)
        {
            continue;
        }

        const std::string &functionName =
            midiSwScreen->getFunctionNames()[functionIndex];

        std::string trimmedFunctionName = functionName;
        trimmedFunctionName.erase(0,
                                  trimmedFunctionName.find_first_not_of(" \t"));
        trimmedFunctionName.erase(trimmedFunctionName.find_last_not_of(" \t") +
                                  1);

        if (trimmedFunctionName.rfind("PAD", 0) == 0)
        {
            ClientHardwareEvent ev;
            ev.source = ClientHardwareEvent::Source::Internal;

            std::string rest = trimmedFunctionName.substr(3);
            rest.erase(0, rest.find_first_not_of(" \t"));
            rest.erase(rest.find_last_not_of(" \t") + 1);

            const int padNum = std::stoi(rest);

            ev.componentId = static_cast<ComponentId>(
                static_cast<int>(ComponentId::PAD_1_OR_AB) + padNum - 1);
            ev.index = padNum - 1;
            ev.type = pressed ? ClientHardwareEvent::Type::PadPress
                              : ClientHardwareEvent::Type::PadRelease;

            if (pressed)
            {
                ev.value = mpc::hardware::Pad::MAX_VELO;
            }

            clientHardwareEventController->handleClientHardwareEvent(ev);
            continue;
        }

        std::optional<ComponentId> id;

        if (functionName == "PLAY STRT")
        {
            id = ComponentId::PLAY_START;
        }
        else if (functionName == "PLAY")
        {
            id = ComponentId::PLAY;
        }
        else if (functionName == "STOP")
        {
            id = ComponentId::STOP;
        }
        else if (functionName == "TAP")
        {
            id = ComponentId::TAP_TEMPO_OR_NOTE_REPEAT;
        }
        else if (functionName == "PAD BNK A")
        {
            id = ComponentId::BANK_A;
        }
        else if (functionName == "PAD BNK B")
        {
            id = ComponentId::BANK_B;
        }
        else if (functionName == "PAD BNK C")
        {
            id = ComponentId::BANK_C;
        }
        else if (functionName == "PAD BNK D")
        {
            id = ComponentId::BANK_D;
        }

        else if (functionName.rfind("F", 0) == 0 ||
                 functionName.find("   F") != std::string::npos)
        {
            // "   F1".."   F6"
            const int fNum =
                std::stoi(functionName.substr(functionName.find('F') + 1));
            id = static_cast<ComponentId>(static_cast<int>(ComponentId::F1) +
                                          fNum - 1);
        }

        if (id)
        {
            ClientHardwareEvent ev;
            ev.componentId = *id;
            ev.source = ClientHardwareEvent::Source::Internal;
            ev.type = pressed ? ClientHardwareEvent::Type::ButtonPress
                              : ClientHardwareEvent::Type::ButtonRelease;
            clientHardwareEventController->handleClientHardwareEvent(ev);
            continue;
        }

        if (functionName == "REC+PLAY")
        {
            if (pressed)
            {
                triggerDualButtonCombo(ComponentId::REC, ComponentId::PLAY);
            }

            continue;
        }
        else if (functionName == "ODUB+PLAY")
        {
            if (pressed)
            {
                triggerDualButtonCombo(ComponentId::OVERDUB, ComponentId::PLAY);
            }

            continue;
        }
        else if (functionName == "REC/PUNCH")
        {
            if (pressed)
            {
                handleRecPunch();
            }
        }
        else if (functionName == "ODUB/PNCH")
        {
            if (pressed)
            {
                handleOdubPunch();
            }
        }
    }
}
