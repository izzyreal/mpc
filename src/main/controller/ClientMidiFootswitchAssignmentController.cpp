#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "sequencer/Transport.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"
#include "sequencer/Sequencer.hpp"

#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::hardware;
using namespace mpc::input::midi;

ClientMidiFootswitchAssignmentController::
    ClientMidiFootswitchAssignmentController(
        const std::shared_ptr<ClientHardwareEventController>
            &clientHardwareEventController,
        const std::shared_ptr<MidiSwScreen> &midiSwScreen,
        const std::weak_ptr<sequencer::Sequencer> &sequencer)
    : clientHardwareEventController(clientHardwareEventController),
      midiSwScreen(midiSwScreen), sequencer(sequencer)
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
        b.target.componentId = PLAY_START; // default placeholder
        bindings.emplace_back(b);
    }
}

void ClientMidiFootswitchAssignmentController::pressButton(ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::MpcButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButton(
    ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::MpcButtonRelease;
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
    pressButton(PLAY);
    releaseButton(PLAY);
}

void ClientMidiFootswitchAssignmentController::handleRecordingToPlay()
{
    pressButton(REC);
    releaseButton(REC);
}

void ClientMidiFootswitchAssignmentController::handleRecPunch()
{
    if (!sequencer.lock()->getTransport()->isPlaying())
    {
        handleStopToPlay();
    }
    else if (sequencer.lock()->getTransport()->isPlaying() &&
             !sequencer.lock()->getTransport()->isRecordingOrOverdubbing())
    {
        triggerDualButtonCombo(REC, PLAY);
    }
    else if (sequencer.lock()->getTransport()->isRecording())
    {
        handleRecordingToPlay();
    }
}

void ClientMidiFootswitchAssignmentController::handleOdubPunch()
{
    if (!sequencer.lock()->getTransport()->isPlaying())
    {
        handleStopToPlay();
    }
    else if (sequencer.lock()->getTransport()->isPlaying() &&
             !sequencer.lock()->getTransport()->isRecordingOrOverdubbing())
    {
        triggerDualButtonCombo(OVERDUB, PLAY);
    }
    else if (sequencer.lock()->getTransport()->isOverdubbing())
    {
        handleRecordingToPlay();
    }
}

void ClientMidiFootswitchAssignmentController::dispatchSequencerCommand(
    const MidiControlTarget::SequencerTarget::Command cmd)
{
    using Cmd = MidiControlTarget::SequencerTarget::Command;
    switch (cmd)
    {
        case Cmd::PLAY:
            handleStopToPlay();
            break;
        case Cmd::REC:
            handleRecordingToPlay();
            break;
        case Cmd::STOP:
            pressButton(STOP);
            releaseButton(STOP);
            break;
        case Cmd::REC_PLUS_PLAY:
            triggerDualButtonCombo(REC, PLAY);
            break;
        case Cmd::ODUB_PLUS_PLAY:
            triggerDualButtonCombo(OVERDUB, PLAY);
            break;
        case Cmd::REC_PUNCH:
            handleRecPunch();
            break;
        case Cmd::ODUB_PUNCH:
            handleOdubPunch();
            break;
    }
}

void ClientMidiFootswitchAssignmentController::handleEvent(
    const ClientMidiEvent &e)
{
    if (e.getMessageType() != ClientMidiEvent::MessageType::CONTROLLER)
    {
        return;
    }

    int number = e.getControllerNumber();
    const int value = e.getControllerValue();
    bool pressed = value >= 64;

    for (auto &binding : bindings)
    {
        std::visit(
            [&](auto &b)
            {
                if (b.number != number)
                {
                    return;
                }

                if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                             HardwareBinding>)
                {
                    if (b.interaction == Interaction::Press)
                    {
                        if (pressed)
                        {
                            printf("Pressing\n");
                            pressButton(b.target.componentId);
                        }
                        else
                        {
                            printf("Releasing\n");
                            releaseButton(b.target.componentId);
                        }
                    }

                    // Optional debug: get footswitch function
                    if (auto fn = controller::componentIdToFootswitch(
                            b.target.componentId))
                    {
                        std::cout << "[MIDI] Footswitch function: "
                                  << static_cast<int>(*fn) << "\n";
                    }
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                                  SequencerBinding>)
                {
                    if (pressed && b.interaction == Interaction::Press)
                    {
                        dispatchSequencerCommand(b.target.command);

                        if (auto fn = controller::sequencerCmdToFootswitch(
                                b.target.command))
                        {
                            std::cout << "[MIDI] Footswitch function: "
                                      << static_cast<int>(*fn) << "\n";
                        }
                    }
                }
            },
            binding);
    }
}
