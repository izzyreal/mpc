#pragma once

#include "inputlogic/ClientInput.h"

#include "controller/ButtonLockTracker.h"

namespace mpc::controller {

class ClientInputControllerBase {
    public:
        virtual ~ClientInputControllerBase() = default;
        virtual void handleAction(const mpc::inputlogic::ClientInput& action) = 0;

        ButtonLockTracker buttonLockTracker;
};

} // namespace mpc::controller
