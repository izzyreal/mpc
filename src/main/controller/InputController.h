#pragma once

#include "InputControllerBase.h"

#include <string>

namespace mpc::inputlogic {
    struct InputAction;
}

namespace mpc {
    class Mpc;
}

namespace mpc::controller {

class InputController : public InputControllerBase {
    public:
        explicit InputController(mpc::Mpc&);
        void handleAction(const mpc::inputlogic::InputAction& action) override;

    private:
        mpc::Mpc &mpc;
        static bool startsWith(const std::string& s, const std::string& prefix);
        static bool endsWith(const std::string& s, const std::string& suffix);

        void handlePadPress(const mpc::inputlogic::InputAction& a);

        void handlePadAftertouch(const mpc::inputlogic::InputAction& a);

        void handleDataWheel(const mpc::inputlogic::InputAction& a);

        void handleSlider(const mpc::inputlogic::InputAction& a);

        void handlePot(const mpc::inputlogic::InputAction& a);

        void handleButtonPress(const mpc::inputlogic::InputAction& a);

        void handleButtonRelease(const mpc::inputlogic::InputAction& a);
    };
} // namespace mpc::controller
