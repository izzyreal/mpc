#pragma once

#include "controller/ClientInputControllerBase.h"

namespace mpc::inputlogic {
    struct ClientInput;
}

namespace mpc {
    class Mpc;
}

namespace mpc::controller {

/* Controller for input going into the client, i.e. the virtualized MPC2000XL */
class ClientInputController : public ClientInputControllerBase {
public:
    explicit ClientInputController(mpc::Mpc&);
    void handleAction(const mpc::inputlogic::ClientInput& action) override;

private:
    mpc::Mpc &mpc;

    void handlePadPress(const mpc::inputlogic::ClientInput& a);
    void handlePadAftertouch(const mpc::inputlogic::ClientInput& a);
    void handlePadRelease(const mpc::inputlogic::ClientInput& a);
    void handleDataWheel(const mpc::inputlogic::ClientInput& a);
    void handleSlider(const mpc::inputlogic::ClientInput& a);
    void handlePot(const mpc::inputlogic::ClientInput& a);
    void handleButtonPress(const mpc::inputlogic::ClientInput& a);
    void handleButtonRelease(const mpc::inputlogic::ClientInput& a);
    void handleButtonDoublePress(const mpc::inputlogic::ClientInput&);
};

} // namespace mpc::controller
