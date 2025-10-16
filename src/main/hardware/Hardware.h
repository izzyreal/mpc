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
    std::vector<std::shared_ptr<Led>> leds;
    std::vector<std::shared_ptr<Pad>> pads;
    std::unordered_map<std::string, std::shared_ptr<Button>> buttons;
    std::shared_ptr<DataWheel> dataWheel;
    std::shared_ptr<Slider> slider;
    std::shared_ptr<Pot> recPot;
    std::shared_ptr<Pot> volPot;

public:
    explicit Hardware()
    {
        for (const auto &label : getButtonLabels())
        {
            buttons[label] = std::make_shared<Button>(label);
        }

        for (int i = 0; i < 16; ++i)
            pads.push_back(std::make_shared<Pad>(i));

        const std::vector<std::string> ledLabels {
            "full-level", "sixteen-levels", "next-seq", "track-mute",
            "bank-a", "bank-b", "bank-c", "bank-d",
            "after", "undo-seq", "rec", "overdub", "play"
        };

        for (const auto& l : ledLabels)
        {
            leds.push_back(std::make_shared<Led>(l));
            if (l == "bank-a") leds.back()->setEnabled(true);
        }

        dataWheel = std::make_shared<DataWheel>();
        recPot = std::make_shared<Pot>();
        volPot = std::make_shared<Pot>();
        slider = std::make_shared<Slider>();
    }

    std::vector<std::string> getButtonLabels()
    {
        std::vector<std::string> result;

        for (auto &[label,id] : componentLabelToId)
        {
            if (id >= ComponentId::CURSOR_LEFT && id <= ComponentId::NUM_9)
            {
                result.push_back(label);
            }
        }
        
        return result;
    }

    std::shared_ptr<Pad> getPad(int index)
    {
        if (index < 0 || index >= static_cast<int>(pads.size())) return nullptr;
        return pads[index];
    }

    std::vector<std::shared_ptr<Pad>>& getPads() { return pads; }

    std::shared_ptr<Button> getButton(const std::string& label) {
        if (auto it = buttons.find(label); it != buttons.end())
            return it->second;
        return nullptr;
    }

    std::vector<std::shared_ptr<Led>> getLeds() { return leds; }

    std::shared_ptr<Led> getLed(const std::string label)
    {
        for (auto &l : leds)
        {
            if (l->getLabel() == label) return l;
        }

        return {};
    }

    std::shared_ptr<DataWheel> getDataWheel() { return dataWheel; }
    std::shared_ptr<Pot> getRecPot() { return recPot; }
    std::shared_ptr<Pot> getVolPot() { return volPot; }
    std::shared_ptr<Slider> getSlider() { return slider; }

    std::shared_ptr<Component> getComponentByLabel(const std::string& label)
    {
        if (auto it = buttons.find(label); it != buttons.end()) return it->second;
        if (label.rfind("pad-", 0) == 0) {
            int index = std::stoi(label.substr(4));
            if (index >= 0 && index < static_cast<int>(pads.size()))
                return pads[index];
        }
        return nullptr;
    }
};

} // namespace mpc::hardware
