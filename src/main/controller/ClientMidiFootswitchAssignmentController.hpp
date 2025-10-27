#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include "hardware/ComponentId.hpp"

#include <memory>

namespace mpc::lcdgui::screens
{
    class MidiSwScreen;
}

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::controller
{
    class ClientHardwareEventController;

    class ClientMidiFootswitchAssignmentController
    {
    public:
        ClientMidiFootswitchAssignmentController(
            std::shared_ptr<ClientHardwareEventController>,
            std::shared_ptr<lcdgui::screens::MidiSwScreen>,
            std::shared_ptr<sequencer::Sequencer>);

        void handleEvent(const mpc::client::event::ClientMidiEvent &);

    private:
        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;
        std::shared_ptr<lcdgui::screens::MidiSwScreen> midiSwScreen;
        std::shared_ptr<sequencer::Sequencer> sequencer;

        void triggerDualButtonCombo(hardware::ComponentId first,
                                    hardware::ComponentId second);
        void handleRecPunch();
        void handleOdubPunch();

        void pressButton(hardware::ComponentId componentId);
        void releaseButton(hardware::ComponentId componentId);
        void handleStopToPlay();
        void handleRecordingToPlay();
    };
} // namespace mpc::controller
