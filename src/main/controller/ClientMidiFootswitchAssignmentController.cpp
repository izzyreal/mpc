#include "controller/ClientMidiFootswitchAssignmentController.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::hardware;
using namespace mpc::midi::input;

ClientMidiFootswitchAssignmentController::
    ClientMidiFootswitchAssignmentController(
        std::shared_ptr<ClientHardwareEventController> ch,
        std::shared_ptr<lcdgui::screens::MidiSwScreen> ms,
        std::shared_ptr<sequencer::Sequencer> seq)
    : clientHardwareEventController(ch), midiSwScreen(ms), sequencer(seq)
{
    bindings.reserve(SWITCH_COUNT);
    initializeDefaultBindings();
}

void ClientMidiFootswitchAssignmentController::initializeDefaultBindings()
{
    bindings.clear();
    for (int i = 0; i < SWITCH_COUNT; i++)
    {
        HardwareBinding b{};
        b.channel = -1; // OMNI
        b.number = -1;  // unassigned
        b.interaction = Interaction::Press;
        b.messageType = MidiBindingBase::MessageType::CC;
        b.target.componentId = ComponentId::PLAY_START; // default placeholder
        bindings.emplace_back(b);
    }
}

void ClientMidiFootswitchAssignmentController::pressButton(ComponentId id)
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::ButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButton(ComponentId id)
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
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
        handleStopToPlay();
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing())
        triggerDualButtonCombo(ComponentId::REC, ComponentId::PLAY);
    else if (sequencer->isRecording())
        handleRecordingToPlay();
}

void ClientMidiFootswitchAssignmentController::handleOdubPunch()
{
    if (!sequencer->isPlaying())
        handleStopToPlay();
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing())
        triggerDualButtonCombo(ComponentId::OVERDUB, ComponentId::PLAY);
    else if (sequencer->isOverdubbing())
        handleRecordingToPlay();
}

void ClientMidiFootswitchAssignmentController::dispatchSequencerCommand(
    MidiControlTarget::SequencerTarget::Command cmd)
{
    using Cmd = MidiControlTarget::SequencerTarget::Command;
    switch (cmd)
    {
        case Cmd::PLAY: handleStopToPlay(); break;
        case Cmd::REC: handleRecordingToPlay(); break;
        case Cmd::STOP: pressButton(ComponentId::STOP); releaseButton(ComponentId::STOP); break;
        case Cmd::REC_PLUS_PLAY: triggerDualButtonCombo(ComponentId::REC, ComponentId::PLAY); break;
        case Cmd::ODUB_PLUS_PLAY: triggerDualButtonCombo(ComponentId::OVERDUB, ComponentId::PLAY); break;
        case Cmd::REC_PUNCH: handleRecPunch(); break;
        case Cmd::ODUB_PUNCH: handleOdubPunch(); break;
    }
}

void ClientMidiFootswitchAssignmentController::handleEvent(const ClientMidiEvent &e)
{
    if (e.getMessageType() != ClientMidiEvent::MessageType::CONTROLLER)
        return;

    int number = e.getControllerNumber();
    int value = e.getControllerValue();
    bool pressed = value >= 64;

    for (auto &binding : bindings)
    {
        std::visit([&](auto &b)
        {
            if (b.number != number) return;

            if constexpr (std::is_same_v<std::decay_t<decltype(b)>, HardwareBinding>)
            {
                if (b.interaction == Interaction::Press)
                {
                    if (pressed) pressButton(b.target.componentId);
                    else releaseButton(b.target.componentId);
                }

                // Optional debug: get footswitch function
                if (auto fn = controller::componentIdToFootswitch(b.target.componentId))
                    std::cout << "[MIDI] Footswitch function: " << static_cast<int>(*fn) << "\n";
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(b)>, SequencerBinding>)
            {
                if (pressed && b.interaction == Interaction::Press)
                {
                    dispatchSequencerCommand(b.target.command);

                    if (auto fn = controller::sequencerCmdToFootswitch(b.target.command))
                        std::cout << "[MIDI] Footswitch function: " << static_cast<int>(*fn) << "\n";
                }
            }
        }, binding);
    }
}

