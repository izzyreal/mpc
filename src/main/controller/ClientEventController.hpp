#pragma once

#include "client/event/ClientEvent.hpp"

#include "input/HostToClientTranslator.hpp"
#include "input/KeyboardBindings.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::controller
{

    class ClientHardwareEventController;

    class ClientEventController
    {

    public:
        ClientEventController(mpc::Mpc &);

        void dispatchHostInput(const mpc::input::HostInputEvent &hostEvent);

        void handleClientEvent(const client::event::ClientEvent &);

        std::shared_ptr<mpc::input::KeyboardBindings> getKeyboardBindings()
        {
            return keyboardBindings;
        }

        std::shared_ptr<ClientHardwareEventController> clientHardwareEventController;

    private:
        std::shared_ptr<mpc::input::KeyboardBindings> keyboardBindings;
    };
} // namespace mpc::controller
