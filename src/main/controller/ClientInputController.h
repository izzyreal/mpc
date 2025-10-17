#pragma once

#include "controller/ClientInputControllerBase.h"

namespace mpc::inputlogic {
    struct ClientInput;
}

namespace mpc {
    class Mpc;
}

namespace mpc::controller {

class ClientInputController : public ClientInputControllerBase {
public:
    explicit ClientInputController(mpc::Mpc&, const fs::path keyboardMappingConfigDirectory);
    void handleAction(const mpc::inputlogic::ClientInput& action) override;

private:
    mpc::Mpc& mpc;

    void handlePadPress(const mpc::inputlogic::ClientInput& a);
    void handlePadAftertouch(const mpc::inputlogic::ClientInput& a);
    void handlePadRelease(const mpc::inputlogic::ClientInput& a);
    void handleDataWheel(const mpc::inputlogic::ClientInput& a);
    void handleSlider(const mpc::inputlogic::ClientInput& a);
    void handlePot(const mpc::inputlogic::ClientInput& a);
    void handleButtonPress(const mpc::inputlogic::ClientInput& a);
    void handleButtonRelease(const mpc::inputlogic::ClientInput& a);
    void handleButtonDoublePress(const mpc::inputlogic::ClientInput& a);
    void handleFocusLost();
};

} // namespace mpc::controller
