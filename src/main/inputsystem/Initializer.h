#pragma once

#include <memory>

namespace mpc::inputlogic {
    class ClientInputMapper;
}

namespace mpc::controller {
    class InputControllerBase;
}

namespace mpc::inputsystem {

struct Initializer {
    static void init(mpc::inputlogic::ClientInputMapper &inputMapper, std::shared_ptr<mpc::controller::InputControllerBase> inputController);
};

} // namespace mpc::inputsystem
