#pragma once

namespace mpc::inputlogic {
    struct InputAction;
}

namespace mpc::controller {
class InputControllerBase {
public:
    virtual ~InputControllerBase() = default;
    virtual void handleAction(const mpc::inputlogic::InputAction& action) = 0;
};
} // namespace mpc::controller
 
