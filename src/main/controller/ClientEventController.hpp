#pragma once

#include "client/event/ClientEvent.hpp"

#include "input/HostToClientTranslator.hpp"
#include "input/KeyboardBindings.hpp"

#include "sequencer/RecordingMode.hpp"
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
}

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::controller
{

    class ClientHardwareEventController;
    class ClientMidiEventController;

    class ClientEventController : public std::enable_shared_from_this<ClientEventController>
    {

    public:
        ClientEventController(mpc::Mpc &);

        void init();

        void dispatchHostInput(const mpc::input::HostInputEvent &hostEvent);

        void handleClientEvent(const client::event::ClientEvent &);

        std::shared_ptr<mpc::input::KeyboardBindings> getKeyboardBindings()
        {
            return keyboardBindings;
        }

        std::shared_ptr<ClientMidiEventController> getClientMidiEventController();

        std::shared_ptr<ClientHardwareEventController> clientHardwareEventController;

        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();

        sequencer::RecordingMode determineRecordingMode() const;

    private:
        mpc::Mpc &mpc;
        std::shared_ptr<ClientMidiEventController> clientMidiEventController;
        std::shared_ptr<mpc::input::KeyboardBindings> keyboardBindings;
        std::shared_ptr<sequencer::Sequencer> sequencer;
        std::shared_ptr<lcdgui::Screens> screens;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<hardware::Hardware> hardware;
    };
} // namespace mpc::controller
