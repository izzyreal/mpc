#include "controller/ClientMidiEventController.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include <iostream>
#include <memory>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

ClientMidiEventController::ClientMidiEventController(std::shared_ptr<ClientHardwareEventController> clientHardwareEventControllerToUse,
                                                     std::shared_ptr<MidiSwScreen> midiSwScreen,
                                                     std::shared_ptr<sequencer::Sequencer> sequencer,
                                                     std::shared_ptr<MidiInputScreen> midiInputScreen,
                                                     std::shared_ptr<audiomidi::EventHandler> eventHandler,
                                                     std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen,
                                                     std::shared_ptr<TimingCorrectScreen> timingCorrectScreen,
                                                     std::shared_ptr<hardware::Button> recButton,
                                                     std::function<std::string()> getCurrentScreenNameToUse)
    : getCurrentScreenName(getCurrentScreenNameToUse), clientHardwareEventController(clientHardwareEventControllerToUse)
{
    footswitchController = std::make_shared<ClientMidiFootswitchAssignmentController>(clientHardwareEventController, midiSwScreen, sequencer);

    // Create the sound generator controller with the passed dependencies
    soundGeneratorController = std::make_shared<ClientMidiSoundGeneratorController>(
        midiInputScreen,
        eventHandler,
        sequencer,
        multiRecordingSetupScreen,
        timingCorrectScreen,
        clientHardwareEventController,
        recButton,
        getCurrentScreenName);
}

void ClientMidiEventController::handleClientMidiEvent(const ClientMidiEvent &e)
{
    e.printInfo();

    if (getCurrentScreenName() == "midi-input-monitor")
    {
        const auto notificationMessage = std::string("a") + std::to_string(e.getChannel());
        notifyObservers(notificationMessage);
    }

    switch (e.getMessageType())
    {
        case ClientMidiEvent::NOTE_ON:
        case ClientMidiEvent::NOTE_OFF:
        case ClientMidiEvent::AFTERTOUCH:
        case ClientMidiEvent::CHANNEL_PRESSURE:
        case ClientMidiEvent::PROGRAM_CHANGE:
        case ClientMidiEvent::PITCH_WHEEL:
            soundGeneratorController->handleEvent(e);
            sequencerController.handleEvent(e);
            break;

        case ClientMidiEvent::CONTROLLER:
            soundGeneratorController->handleEvent(e);
            sequencerController.handleEvent(e);
            footswitchController->handleEvent(e);
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

std::shared_ptr<ClientMidiSoundGeneratorController> ClientMidiEventController::getSoundGeneratorController()
{
    return soundGeneratorController;
}
