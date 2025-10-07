#pragma once

namespace mpc::inputlogic {
    class InputMapper;
}

namespace mpc::controller {
    class InputController;
}

namespace mpc::inputsystem {

struct Initializer {
    static void init(mpc::inputlogic::InputMapper &inputMapper, mpc::controller::InputController &inputController);
};

} // namespace mpc::inputsystem
