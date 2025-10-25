#include "controller/ClientMidiInputSynchronizationController.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;

void ClientMidiInputSynchronizationController::handleEvent(const ClientMidiEvent &e)
{
    std::cout << "[InputSync] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;
}
