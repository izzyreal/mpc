#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "sequencer/Transport.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "utils/VariantUtils.hpp"

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

void ClientMidiFootswitchAssignmentController::pressButtonOrPad(
    const ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;

    const auto isPad = isPadId(id);

    ev.type = isPad ? ClientHardwareEvent::Type::PadPress
                    : ClientHardwareEvent::Type::MpcButtonPress;

    if (isPad)
    {
        ev.index = ev.index =
            static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
        ev.value = 1.f;
    }

    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButtonOrPad(
    const ComponentId id) const
{
    ClientHardwareEvent ev{};
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    const auto isPad = isPadId(id);

    ev.type = isPad ? ClientHardwareEvent::Type::PadRelease
                    : ClientHardwareEvent::Type::MpcButtonRelease;

    if (isPad)
    {
        ev.index = ev.index =
            static_cast<int>(id) - static_cast<int>(PAD_1_OR_AB);
    }

    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::triggerDualButtonCombo(
    const ComponentId first, const ComponentId second) const
{
    pressButtonOrPad(first);
    pressButtonOrPad(second);
    releaseButtonOrPad(second);
    releaseButtonOrPad(first);
}

void ClientMidiFootswitchAssignmentController::handleStopToPlay() const
{
    pressButtonOrPad(PLAY);
    releaseButtonOrPad(PLAY);
}

void ClientMidiFootswitchAssignmentController::handleRecordingToPlay() const
{
    pressButtonOrPad(REC);
    releaseButtonOrPad(REC);
}

void ClientMidiFootswitchAssignmentController::handleRecPunch() const
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

void ClientMidiFootswitchAssignmentController::handleOdubPunch() const
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
    const MidiControlTarget::SequencerTarget::Command cmd) const
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
            pressButtonOrPad(STOP);
            releaseButtonOrPad(STOP);
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

    const int number = e.getControllerNumber();
    const int value = e.getControllerValue();
    const bool pressed = value >= 64;

    auto visitor = Overload{
        [&](const HardwareBinding &b)
        {
            if (b.interaction == Interaction::Press)
            {
                if (pressed)
                {
                    pressButtonOrPad(b.target.componentId);
                }
                else
                {
                    releaseButtonOrPad(b.target.componentId);
                }
            }
        },
        [&](const SequencerBinding &b)
        {
            if (pressed && b.interaction == Interaction::Press)
            {
                dispatchSequencerCommand(b.target.command);
            }
        }};

    for (auto &binding : bindings)
    {
        if (std::visit(
                [](auto const &b)
                {
                    return b.number;
                },
                binding) != number)
        {
            continue;
        }

        std::visit(visitor, binding);
    }
}
