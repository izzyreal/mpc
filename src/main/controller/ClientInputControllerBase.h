#pragma once

#include "inputlogic/ClientInput.h"

namespace mpc::controller {

class ClientInputControllerBase {
public:
    virtual ~ClientInputControllerBase() = default;
    virtual void handleAction(const mpc::inputlogic::ClientInput& action) = 0;
};

} // namespace mpc::controller