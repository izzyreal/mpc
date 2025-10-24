#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "hardware/ComponentId.hpp"
#include "hardware/Component.hpp"

namespace mpc::hardware
{

    class Hardware final
    {
    private:
        std::unordered_map<ComponentId, std::shared_ptr<Component>> components;
        std::unordered_map<ComponentId, std::shared_ptr<Led>> leds;
        std::vector<std::shared_ptr<Pad>> pads;
        std::unordered_map<ComponentId, std::shared_ptr<Button>> buttons;
        std::shared_ptr<DataWheel> dataWheel;
        std::shared_ptr<Slider> slider;
        std::shared_ptr<Pot> recPot;
        std::shared_ptr<Pot> volPot;

    public:
        explicit Hardware();

        std::vector<std::string> getButtonLabels();
        std::shared_ptr<Pad> getPad(int index);
        std::vector<std::shared_ptr<Pad>> &getPads();
        std::shared_ptr<Button> getButton(ComponentId id);
        std::shared_ptr<Led> getLed(ComponentId id);
        std::shared_ptr<DataWheel> getDataWheel();
        std::shared_ptr<Pot> getRecPot();
        std::shared_ptr<Pot> getVolPot();
        std::shared_ptr<Slider> getSlider();
        std::shared_ptr<Component> getComponent(ComponentId id);
    };

} // namespace mpc::hardware
