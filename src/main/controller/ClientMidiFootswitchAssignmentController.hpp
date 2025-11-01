#pragma once

#include "client/event/ClientMidiEvent.hpp"
#include "hardware/ComponentId.hpp"
#include "midi/input/MidiControlTargetBinding.hpp"

#include <memory>
#include <vector>

namespace mpc::lcdgui::screens { class MidiSwScreen; }
namespace mpc::sequencer { class Sequencer; }

namespace mpc::controller {

class ClientHardwareEventController;

class ClientMidiFootswitchAssignmentController {
public:
    ClientMidiFootswitchAssignmentController(
        std::shared_ptr<ClientHardwareEventController>,
        std::shared_ptr<lcdgui::screens::MidiSwScreen>,
        std::shared_ptr<sequencer::Sequencer>);

    void handleEvent(const mpc::client::event::ClientMidiEvent &);

private:
    std::shared_ptr<ClientHardwareEventController> clientHardwareEventController;
    std::shared_ptr<lcdgui::screens::MidiSwScreen> midiSwScreen;
    std::shared_ptr<sequencer::Sequencer> sequencer;

    std::vector<mpc::midi::input::MidiControlTargetBinding> bindings;

    void dispatchSequencerCommand(mpc::midi::input::MidiControlTarget::SequencerTarget::Command);

    void triggerDualButtonCombo(mpc::hardware::ComponentId, mpc::hardware::ComponentId);
    void handleRecPunch();
    void handleOdubPunch();
    void pressButton(mpc::hardware::ComponentId);
    void releaseButton(mpc::hardware::ComponentId);
    void handleStopToPlay();
    void handleRecordingToPlay();
};

} // namespace mpc::controller

