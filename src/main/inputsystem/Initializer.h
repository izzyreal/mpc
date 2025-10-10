#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mpc::inputlogic {
    class ClientInputMapper;
    struct ClientInput;
}

namespace mpc::controller {
    class ClientInputControllerBase;
}

namespace mpc::inputsystem {
    struct Initializer {
        static void init(mpc::inputlogic::ClientInputMapper &inputMapper,
                         std::shared_ptr<mpc::controller::ClientInputControllerBase> inputController,
                         const std::vector<std::string> buttonLabels);
    };
}
