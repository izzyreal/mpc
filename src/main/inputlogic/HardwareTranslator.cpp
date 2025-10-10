#include "inputlogic/HardwareTranslator.h"

#include "inputlogic/InputAction.h"

using namespace mpc::inputlogic;

InputAction HardwareTranslator::fromDataWheelTurn(int steps)
{
    return InputAction{ steps > 0 ? "datawheel-up" : "datawheel-down", steps };
}

InputAction HardwareTranslator::fromPadPress(int padIndex, int velocity)
{
    return InputAction{ "pad-" + std::to_string(padIndex) + "-press", velocity };
}

InputAction HardwareTranslator::fromPadRelease(int padIndex)
{
    return InputAction{ "pad-" + std::to_string(padIndex) + "-release", std::nullopt };
}

InputAction HardwareTranslator::fromPadAftertouch(int padIndex, int pressure)
{
    return InputAction{ "pad-" + std::to_string(padIndex) + "-aftertouch", pressure };
}

InputAction HardwareTranslator::fromSliderMove(int value)
{
    return InputAction{ "slider", value };
}

InputAction HardwareTranslator::fromButtonPress(const std::string& label)
{
    return InputAction{ label + "-press", std::nullopt };
}

InputAction HardwareTranslator::fromButtonRelease(const std::string& label)
{
    return InputAction{ label + "-release", std::nullopt };
}

