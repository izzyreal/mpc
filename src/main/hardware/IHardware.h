#pragma once
#include <memory>
#include <string>
#include <vector>

namespace mpc::hardware {
class HwPad;
class DataWheel;
class Slider;
class Pot;
class HwComponent;

struct IHardware {
    virtual ~IHardware() = default;

    virtual std::vector<std::shared_ptr<mpc::hardware::HwPad>>& getPads() = 0;
    virtual std::shared_ptr<mpc::hardware::DataWheel> getDataWheel() = 0;
    virtual std::shared_ptr<mpc::hardware::Slider> getSlider() = 0;
    virtual std::shared_ptr<mpc::hardware::Pot> getRecPot() = 0;
    virtual std::shared_ptr<mpc::hardware::Pot> getVolPot() = 0;
    virtual std::shared_ptr<mpc::hardware::HwComponent> getComponentByLabel(const std::string& label) = 0;
};
} // namespace mpc::hardware
