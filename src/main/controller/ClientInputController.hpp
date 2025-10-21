#pragma once

#include "controller/ClientInputControllerBase.hpp"

namespace mpc::input {
    struct ClientInput;
}

namespace mpc {
    class Mpc;
}

namespace mpc::controller {

class ClientInputController : public ClientInputControllerBase {
public:
    explicit ClientInputController(mpc::Mpc&, const fs::path keyboardMappingConfigDirectory);
    void handleInput(const mpc::input::ClientInput&) override;

private:
    mpc::Mpc& mpc;

    void handlePadPress(const mpc::input::ClientInput& a);
    void handlePadAftertouch(const mpc::input::ClientInput& a);
    void handlePadRelease(const mpc::input::ClientInput& a);
    void handleDataWheel(const mpc::input::ClientInput& a);
    void handleSlider(const mpc::input::ClientInput& a);
    void handlePot(const mpc::input::ClientInput& a);
    void handleButtonPress(const mpc::input::ClientInput& a);
    void handleButtonRelease(const mpc::input::ClientInput& a);
    void handleButtonDoublePress(const mpc::input::ClientInput& a);
    void handleFocusLost();
};

} // namespace mpc::controller
