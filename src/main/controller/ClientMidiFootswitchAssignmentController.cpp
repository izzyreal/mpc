#include "controller/ClientMidiFootswitchAssignmentController.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;

void ClientMidiFootswitchAssignmentController::handleEvent(const ClientMidiEvent &e)
{
    std::cout << "[FootswitchAssignment] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;
}
