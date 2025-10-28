#include "controller/ClientMidiSequencerController.hpp"

#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;

void ClientMidiSequencerController::handleEvent(const ClientMidiEvent &e)
{
    std::cout << "[Sequencer] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;
}
