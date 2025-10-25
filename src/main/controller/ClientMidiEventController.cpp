#include "controller/ClientMidiEventController.hpp"
#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;

ClientMidiEventController::ClientMidiEventController()
{
}

void ClientMidiEventController::handleClientMidiEvent(const ClientMidiEvent &e)
{
    printf("foo\n");
    e.printInfo();

    switch (e.getMessageType())
    {
    case ClientMidiEvent::NOTE_ON:
    case ClientMidiEvent::NOTE_OFF:
    case ClientMidiEvent::AFTERTOUCH:
    case ClientMidiEvent::CHANNEL_PRESSURE:
    case ClientMidiEvent::PROGRAM_CHANGE:
    case ClientMidiEvent::PITCH_WHEEL:
        soundGeneratorController.handleEvent(e);
        sequencerController.handleEvent(e);
        break;

    case ClientMidiEvent::CONTROLLER:
        soundGeneratorController.handleEvent(e);
        sequencerController.handleEvent(e);
        footswitchController.handleEvent(e);
        break;

    case ClientMidiEvent::MIDI_CLOCK:
    case ClientMidiEvent::MIDI_START:
    case ClientMidiEvent::MIDI_STOP:
    case ClientMidiEvent::MIDI_CONTINUE:
        syncController.handleEvent(e);
        break;

    default:
        std::cout << "[EventController] Unhandled MIDI event type "
                  << e.getMessageType() << std::endl;
        break;
    }
}

