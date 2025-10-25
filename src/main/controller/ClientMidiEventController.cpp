#include "controller/ClientMidiEventController.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;

ClientMidiEventController::ClientMidiEventController()
{
}

void handleClientMidiEvent(const ClientMidiEvent &e)
{
    e.printInfo();
}

