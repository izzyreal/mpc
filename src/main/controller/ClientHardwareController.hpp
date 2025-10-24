#pragma once

#include "controller/ClientHardwareControllerBase.hpp"
#include "client/event/ClientHardwareEvent.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::controller
{

    class ClientHardwareController : public ClientHardwareControllerBase
    {
    public:
        explicit ClientHardwareController(mpc::Mpc &, const fs::path keyboardMappingConfigDirectory);
        void handleInput(const client::event::ClientHardwareEvent &) override;

    private:
        mpc::Mpc &mpc;

        void handlePadPress(const client::event::ClientHardwareEvent &);
        void handlePadAftertouch(const client::event::ClientHardwareEvent &);
        void handlePadRelease(const client::event::ClientHardwareEvent &);
        void handleDataWheel(const client::event::ClientHardwareEvent &);
        void handleSlider(const client::event::ClientHardwareEvent &);
        void handlePot(const client::event::ClientHardwareEvent &);
        void handleButtonPress(const client::event::ClientHardwareEvent &);
        void handleButtonRelease(const client::event::ClientHardwareEvent &);
        void handleButtonDoublePress(const client::event::ClientHardwareEvent &);
    };

} // namespace mpc::controller
