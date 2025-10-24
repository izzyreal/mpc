#pragma once

#include "controller/ClientHardwareControllerBase.hpp"

namespace mpc::input {
    struct ClientHardwareEvent;
}

namespace mpc {
    class Mpc;
}

namespace mpc::controller {

class ClientHardwareController : public ClientHardwareControllerBase {
public:
    explicit ClientHardwareController(mpc::Mpc&, const fs::path keyboardMappingConfigDirectory);
    void handleInput(const mpc::input::ClientHardwareEvent&) override;

private:
    mpc::Mpc& mpc;

    void handlePadPress(const mpc::input::ClientHardwareEvent& a);
    void handlePadAftertouch(const mpc::input::ClientHardwareEvent& a);
    void handlePadRelease(const mpc::input::ClientHardwareEvent& a);
    void handleDataWheel(const mpc::input::ClientHardwareEvent& a);
    void handleSlider(const mpc::input::ClientHardwareEvent& a);
    void handlePot(const mpc::input::ClientHardwareEvent& a);
    void handleButtonPress(const mpc::input::ClientHardwareEvent& a);
    void handleButtonRelease(const mpc::input::ClientHardwareEvent& a);
    void handleButtonDoublePress(const mpc::input::ClientHardwareEvent& a);
};

} // namespace mpc::controller
