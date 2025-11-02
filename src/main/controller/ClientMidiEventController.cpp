#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientEventController.hpp"

#include <memory>

using namespace mpc::eventregistry;
using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::hardware;
using namespace mpc::engine;

ClientMidiEventController::ClientMidiEventController(
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<ClientEventController> clientEventController,
    std::shared_ptr<ClientHardwareEventController>
        clientHardwareEventController,
    std::shared_ptr<MidiSwScreen> midiSwScreen,
    std::shared_ptr<Sequencer> sequencer, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<MidiInputScreen> midiInputScreen,
    std::shared_ptr<audiomidi::EventHandler> eventHandler,
    std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen,
    std::shared_ptr<TimingCorrectScreen> timingCorrectScreen,
    std::shared_ptr<LayeredScreen> layeredScreen,
    std::shared_ptr<Hardware> hardware, std::shared_ptr<Screens> screens,
    std::shared_ptr<FrameSeq> frameSequencer,
    PreviewSoundPlayer *previewSoundPlayer)
    : clientEventController(clientEventController),
      clientHardwareEventController(clientHardwareEventController)
{
    footswitchController =
        std::make_shared<ClientMidiFootswitchAssignmentController>(
            clientHardwareEventController, midiSwScreen, sequencer);

    soundGeneratorController =
        std::make_shared<ClientMidiSoundGeneratorController>(
            eventRegistry, clientEventController, midiInputScreen, eventHandler,
            sequencer, sampler, multiRecordingSetupScreen, timingCorrectScreen,
            layeredScreen, hardware, screens, frameSequencer,
            previewSoundPlayer);
}

void ClientMidiEventController::handleClientMidiEvent(const ClientMidiEvent &e)
{
    if (clientEventController->getLayeredScreen()
            ->isCurrentScreen<MidiInputMonitorScreen>())
    {
        const auto notificationMessage =
            std::string("a") + std::to_string(e.getChannel());
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
            break;
    }
}

std::shared_ptr<ClientMidiSoundGeneratorController>
ClientMidiEventController::getSoundGeneratorController()
{
    return soundGeneratorController;
}

std::shared_ptr<ClientMidiFootswitchAssignmentController>
ClientMidiEventController::getFootswitchAssignmentController()
{
    return footswitchController;
}
