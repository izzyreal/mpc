#include "inputlogic/ClientInputMapper.h"
#include "inputlogic/ClientInput.h"

using namespace mpc::inputlogic;

void ClientInputMapper::bindPadPress(int index, Callback callback)
{
    if (index < 0) return;
    if (padPressBindings.empty()) {
        padPressBindings.resize(MAX_PADS);
        padReleaseBindings.resize(MAX_PADS);
        padAftertouchBindings.resize(MAX_PADS);
    }
    if (index >= static_cast<int>(padPressBindings.size())) padPressBindings.resize(index + 1);
    padPressBindings[index] = std::move(callback);
}

void ClientInputMapper::bindPadRelease(int index, Callback callback)
{
    if (index < 0) return;
    if (padReleaseBindings.empty()) {
        padPressBindings.resize(MAX_PADS);
        padReleaseBindings.resize(MAX_PADS);
        padAftertouchBindings.resize(MAX_PADS);
    }
    if (index >= static_cast<int>(padReleaseBindings.size())) padReleaseBindings.resize(index + 1);
    padReleaseBindings[index] = std::move(callback);
}

void ClientInputMapper::bindPadAftertouch(int index, Callback callback)
{
    if (index < 0) return;
    if (padAftertouchBindings.empty()) {
        padPressBindings.resize(MAX_PADS);
        padReleaseBindings.resize(MAX_PADS);
        padAftertouchBindings.resize(MAX_PADS);
    }
    if (index >= static_cast<int>(padAftertouchBindings.size())) padAftertouchBindings.resize(index + 1);
    padAftertouchBindings[index] = std::move(callback);
}

void ClientInputMapper::bindButtonPress(const std::string& label, Callback callback)
{
    buttonPressBindings[label] = std::move(callback);
}

void ClientInputMapper::bindButtonRelease(const std::string& label, Callback callback)
{
    buttonReleaseBindings[label] = std::move(callback);
}

void ClientInputMapper::bindDataWheel(Callback callback)
{
    dataWheelBinding = std::move(callback);
}

void ClientInputMapper::bindSlider(Callback callback)
{
    sliderBinding = std::move(callback);
}

void ClientInputMapper::bindPot(Callback callback)
{
    potBinding = std::move(callback);
}

void ClientInputMapper::trigger(const ClientInput& action) const
{
    switch (action.type) {
    case ClientInput::Type::PadPress:
        if (action.index && action.index.value() >= 0
            && action.index.value() < static_cast<int>(padPressBindings.size())
            && padPressBindings[action.index.value()].has_value()) {
            padPressBindings[action.index.value()].value()(action);
        }
        break;
    case ClientInput::Type::PadRelease:
        if (action.index && action.index.value() >= 0
            && action.index.value() < static_cast<int>(padReleaseBindings.size())
            && padReleaseBindings[action.index.value()].has_value()) {
            padReleaseBindings[action.index.value()].value()(action);
        }
        break;
    case ClientInput::Type::PadAftertouch:
        if (action.index && action.index.value() >= 0
            && action.index.value() < static_cast<int>(padAftertouchBindings.size())
            && padAftertouchBindings[action.index.value()].has_value()) {
            padAftertouchBindings[action.index.value()].value()(action);
        }
        break;
    case ClientInput::Type::DataWheelTurn:
        if (dataWheelBinding.has_value()) dataWheelBinding.value()(action);
        break;
    case ClientInput::Type::SliderMove:
        if (sliderBinding.has_value()) sliderBinding.value()(action);
        break;
    case ClientInput::Type::PotMove:
        if (potBinding.has_value()) potBinding.value()(action);
        break;
    case ClientInput::Type::ButtonPress:
        if (action.label) {
            auto it = buttonPressBindings.find(action.label.value());
            if (it != buttonPressBindings.end()) it->second(action);
        }
        break;
    case ClientInput::Type::ButtonRelease:
        if (action.label) {
            auto it = buttonReleaseBindings.find(action.label.value());
            if (it != buttonReleaseBindings.end()) it->second(action);
        }
        break;
    default:
        // unknown: ignore
        break;
    }
}
