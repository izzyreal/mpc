#pragma once

namespace mpc::inputlogic {
    struct ClientInputAction;
}

namespace mpc::controller {
class ClientInputControllerBase {
public:
    virtual ~ClientInputControllerBase() = default;
    virtual void handleAction(const mpc::inputlogic::ClientInputAction& action) = 0;
};
} // namespace mpc::controller
 
