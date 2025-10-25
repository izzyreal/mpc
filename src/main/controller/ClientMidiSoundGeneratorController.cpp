#include "controller/ClientMidiSoundGeneratorController.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;

void ClientMidiSoundGeneratorController::handleEvent(const ClientMidiEvent &e)
{
    std::cout << "[SoundGenerator] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;
}
