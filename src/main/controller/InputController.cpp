#include "InputController.h"

#include "inputlogic/InputAction.h"

using namespace mpc::controller;
using namespace mpc::inputlogic; 

void InputController::handleAction(const InputAction& action)
{
    if (startsWith(action.id, "pad-") && endsWith(action.id, "-press")) return handlePadPress(action);
    if (startsWith(action.id, "pad-") && endsWith(action.id, "-aftertouch")) return handlePadAftertouch(action);
    if (startsWith(action.id, "datawheel-")) return handleDataWheel(action);
    if (action.id == "slider-move") return handleSlider(action);
    if (action.id == "pot-move") return handlePot(action);
    if (endsWith(action.id, "-press")) return handleButtonPress(action);
    if (endsWith(action.id, "-release")) return handleButtonRelease(action);
}

bool InputController::startsWith(const std::string& s, const std::string& prefix)
{
    return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
}

bool InputController::endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

void InputController::handlePadPress(const InputAction& a)
{
    const auto num = std::stoi(a.id.substr(4, a.id.find("-press") - 4));
    std::printf("[logic] pad %d pressed with velocity %d\n", num, a.value.value_or(100));
}

void InputController::handlePadAftertouch(const InputAction& a)
{
    const auto num = std::stoi(a.id.substr(4, a.id.find("-aftertouch") - 4));
    std::printf("[logic] pad %d aftertouch pressure %d\n", num, a.value.value_or(0));
}

void InputController::handleDataWheel(const InputAction& a)
{
    const char* dir = a.id == "datawheel-up" ? "up" : "down";
    std::printf("[logic] data wheel turned %s (%d)\n", dir, a.value.value_or(0));
}

void InputController::handleSlider(const InputAction& a)
{
    std::printf("[logic] slider moved to %d\n", a.value.value_or(0));
}

void InputController::handlePot(const InputAction& a)
{
    std::printf("[logic] pot moved to %d\n", a.value.value_or(0));
}

void InputController::handleButtonPress(const InputAction& a)
{
    std::printf("[logic] button %s pressed\n", a.id.c_str());
}

void InputController::handleButtonRelease(const InputAction& a)
{
    std::printf("[logic] button %s released\n", a.id.c_str());
}

