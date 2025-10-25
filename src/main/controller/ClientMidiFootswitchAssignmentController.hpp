#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include "hardware/ComponentId.hpp"

#include <memory>

namespace mpc::lcdgui::screens
{
    class MidiSwScreen;
}

namespace mpc::controller
{
    class ClientHardwareEventController;

    class ClientMidiFootswitchAssignmentController
    {
    public:
        ClientMidiFootswitchAssignmentController(
                std::shared_ptr<ClientHardwareEventController>,
                std::shared_ptr<lcdgui::screens::MidiSwScreen>);

        void handleEvent(const mpc::client::event::ClientMidiEvent &);

    private:
        std::shared_ptr<ClientHardwareEventController> clientHardwareEventController;
        std::shared_ptr<lcdgui::screens::MidiSwScreen> midiSwScreen;

        void triggerDualButtonCombo(hardware::ComponentId first, hardware::ComponentId second);
    };
} // namespace mpc::controller
