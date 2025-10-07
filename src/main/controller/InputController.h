#pragma once

#include <string>

namespace mpc::inputlogic {
    class InputAction;
}

namespace mpc::controller {

class InputController {
    public:
        static void handleAction(const mpc::inputlogic::InputAction& action);

    private:
        static bool startsWith(const std::string& s, const std::string& prefix);
        static bool endsWith(const std::string& s, const std::string& suffix);

        static void handlePadPress(const mpc::inputlogic::InputAction& a);

        static void handlePadAftertouch(const mpc::inputlogic::InputAction& a);

        static void handleDataWheel(const mpc::inputlogic::InputAction& a);

        static void handleSlider(const mpc::inputlogic::InputAction& a);

        static void handlePot(const mpc::inputlogic::InputAction& a);

        static void handleButtonPress(const mpc::inputlogic::InputAction& a);

        static void handleButtonRelease(const mpc::inputlogic::InputAction& a);
    };
} // namespace mpc::controller
