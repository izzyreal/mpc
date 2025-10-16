#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "hardware/ComponentId.h"
#include "hardware/HardwareComponent.h"

#include "Mpc.hpp"

namespace mpc::hardware {

class Hardware final {
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
    explicit Hardware()
    {
        for (int i = ComponentId::CURSOR_LEFT_OR_DIGIT; i <= ComponentId::NUM_9_OR_MIDI_SYNC; i++)
        {
            const auto id = static_cast<ComponentId>(i);
            buttons[id] = std::make_shared<Button>(id);
            components[id] = buttons[id];
        }

        for (int i = 0; i < 16; ++i)
        {
            pads.push_back(std::make_shared<Pad>(i));
            components[pads.back()->getId()] = pads.back();
        }

        for (int i = ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED; i <= ComponentId::PLAY_LED; i++)
        {
            const auto id = static_cast<ComponentId>(i);
            leds[id] = std::make_shared<Led>(id);
            components[id] = leds[id];
            if (id == ComponentId::BANK_A_LED) leds[id]->setEnabled(true);
        }

        dataWheel = std::make_shared<DataWheel>();
        components[dataWheel->getId()] = dataWheel;

        recPot = std::make_shared<Pot>(ComponentId::REC_GAIN_POT);
        components[recPot->getId()] = recPot;

        volPot = std::make_shared<Pot>(ComponentId::MAIN_VOLUME_POT);
        components[volPot->getId()] = volPot;

        slider = std::make_shared<Slider>();
        components[slider->getId()] = slider;
    }

    std::vector<std::string> getButtonLabels()
    {
        std::vector<std::string> result;

        for (auto &[label,id] : componentLabelToId)
        {
            if (id >= ComponentId::CURSOR_LEFT_OR_DIGIT && id <= ComponentId::NUM_9_OR_MIDI_SYNC)
            {
                result.push_back(label);
            }
        }
        
        return result;
    }

    std::shared_ptr<Pad> getPad(int index)
    {
        if (index < 0 || index >= static_cast<int>(pads.size())) return {};
        return pads[index];
    }

    std::vector<std::shared_ptr<Pad>>& getPads() { return pads; }

    std::shared_ptr<Button> getButton(const ComponentId id)
    {
        if (buttons.count(id) == 0) return {};
        return buttons[id];
    }

    std::shared_ptr<Led> getLed(ComponentId id)
    {
        if (leds.count(id) == 0) return {};
        return leds[id];
    }

    std::shared_ptr<DataWheel> getDataWheel() { return dataWheel; }
    std::shared_ptr<Pot> getRecPot() { return recPot; }
    std::shared_ptr<Pot> getVolPot() { return volPot; }
    std::shared_ptr<Slider> getSlider() { return slider; }

    std::shared_ptr<Component> getComponent(const ComponentId id)
    {
        if (components.count(id) == 0) return {};
        return components[id];
    }
};

} // namespace mpc::hardware
