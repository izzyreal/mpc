#pragma once

#include <string>

namespace mpc::inputlogic {
    struct InputAction;
    class HardwareTranslator {
        public:
            static InputAction fromDataWheelTurn(int steps);

            static InputAction fromPadPress(int padIndex, int velocity);

            static InputAction fromPadRelease(int padIndex);

            static InputAction fromPadAftertouch(int padIndex, int pressure);

            static InputAction fromSliderMove(int value);

            static InputAction fromButtonPress(const std::string& label);

            static InputAction fromButtonRelease(const std::string& label);
    };
} // namespace mpc::inputlogic
