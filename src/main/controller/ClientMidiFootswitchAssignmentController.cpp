#include "controller/ClientMidiFootswitchAssignmentController.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
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

        // Default target: unassigned pad (or could use PLAY as placeholder)
        b.target.componentId = mpc::hardware::ComponentId::PLAY_START;

        bindings.emplace_back(b);
    }
}

void ClientMidiFootswitchAssignmentController::pressButton(ComponentId id)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::ButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButton(ComponentId id)
{
    ClientHardwareEvent ev;
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
    {
        handleStopToPlay();
    }
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing())
    {
        pressButton(ComponentId::REC);
        pressButton(ComponentId::PLAY);
        releaseButton(ComponentId::PLAY);
        releaseButton(ComponentId::REC);
    }
    else if (sequencer->isRecording())
    {
        handleRecordingToPlay();
    }
}

void ClientMidiFootswitchAssignmentController::handleOdubPunch()
{
    if (!sequencer->isPlaying())
    {
        handleStopToPlay();
    }
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing())
    {
        pressButton(ComponentId::OVERDUB);
        pressButton(ComponentId::PLAY);
        releaseButton(ComponentId::PLAY);
        releaseButton(ComponentId::OVERDUB);
    }
    else if (sequencer->isOverdubbing())
    {
        handleRecordingToPlay();
    }
}

void ClientMidiFootswitchAssignmentController::dispatchSequencerCommand(
    MidiControlTarget::SequencerTarget::Command cmd)
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
            pressButton(ComponentId::STOP);
            releaseButton(ComponentId::STOP);
            break;
        case Cmd::REC_PLUS_PLAY:
            triggerDualButtonCombo(ComponentId::REC, ComponentId::PLAY);
            break;
        case Cmd::ODUB_PLUS_PLAY:
            triggerDualButtonCombo(ComponentId::OVERDUB, ComponentId::PLAY);
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
    std::cout << "[MIDI] handleEvent called\n";
    e.printInfo();

    if (e.getMessageType() != ClientMidiEvent::MessageType::CONTROLLER)
    {
        std::cout << "[MIDI] Not a controller message, ignoring\n";
        return;
    }

    // int channel = e.getChannel();
    int number = e.getControllerNumber();
    int value = e.getControllerValue();
    bool pressed = value >= 64;

    std::cout /*<< "[MIDI] Channel=" << channel*/
        << " CC=" << number << " Value=" << value << " Pressed=" << pressed
        << "\n";

    int checked = 0;
    int matched = 0;

    for (auto &binding : bindings)
    {
        ++checked;
        std::visit(
            [&](auto &b)
            {
                std::cout << "  [Binding " << checked << "] "
                          << "ch=" << b.channel << " num=" << b.number
                          << " interaction=" << static_cast<int>(b.interaction)
                          << "\n";

                if (b.number != number)
                {
                    return;
                }

                ++matched;
                std::cout << "    → Match found. Pressed=" << pressed << "\n";

                if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                             HardwareBinding>)
                {
                    if (b.interaction == Interaction::Press)
                    {
                        if (pressed)
                        {
                            std::cout << "    → ButtonPress for "
                                      << static_cast<int>(b.target.componentId)
                                      << "\n";
                            pressButton(b.target.componentId);
                        }
                        else
                        {
                            std::cout << "    → ButtonRelease for "
                                      << static_cast<int>(b.target.componentId)
                                      << "\n";
                            releaseButton(b.target.componentId);
                        }
                    }
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                                  SequencerBinding>)
                {
                    if (pressed && b.interaction == Interaction::Press)
                    {
                        std::cout
                            << "    → Dispatching SequencerBinding command\n";
                        dispatchSequencerCommand(b.target.command);
                    }
                }
            },
            binding);
    }
    std::cout << "[MIDI] Checked " << checked << " bindings, matched "
              << matched << "\n";
}
