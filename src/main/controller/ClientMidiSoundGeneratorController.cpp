#include "controller/ClientMidiSoundGeneratorController.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;

void ClientMidiSoundGeneratorController::handleEvent(const ClientMidiEvent &e)
{
    using MessageType = ClientMidiEvent::MessageType;

    std::cout << "[SoundGenerator] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;

    const auto type = e.getMessageType();

    if (type == MessageType::NOTE_ON)
    {
        // TODO: trigger note if within 35–98 range
    }
    else if (type == MessageType::NOTE_OFF)
    {

    }
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 7)
    {
        // TODO: handle CC7 / note variation slider
    }
    else if (type == MessageType::CHANNEL_PRESSURE)
    {
        // TODO: handle channel pressure (affects sound)
    }
    else if (type == MessageType::AFTERTOUCH)
    {
    }
    else if (type == MessageType::PROGRAM_CHANGE)
    {
        // TODO: handle program (sound bank) change
    }
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 123)
    {
        // TODO: handle all notes off
    }
}

