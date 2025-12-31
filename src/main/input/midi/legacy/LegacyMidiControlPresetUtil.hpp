#pragma once

#include "hardware/ComponentId.hpp"

#include <string>

namespace mpc::input::midi::legacy
{
    inline std::string mapLegacyLabelToHardwareTarget(const std::string &label)
    {
        if (label.substr(0, 4) == "pad-")
        {
            std::string digitStr = label.substr(4, 1);
            if (label.length() > 5)
            {
                digitStr += label.substr(5, 1);
            }
            int padNumber = std::stoi(digitStr);
            return "hardware:" +
                   hardware::componentIdToLabel.at(
                       static_cast<hardware::ComponentId>(
                           hardware::PAD_1_OR_AB + (padNumber - 1)));
        }

        if (label == "datawheel-up") return "hardware:data-wheel:positive";
        if (label == "datawheel-down") return "hardware:data-wheel:negative";

        if (label == "up") return "hardware:cursor-up";
        if (label == "down") return "hardware:cursor-down";
        if (label == "left") return "hardware:cursor-left-or-digit";
        if (label == "right") return "hardware:cursor-right-or-digit";
        if (label == "prev-step-event") return "hardware:prev-step-or-event";
        if (label == "next-step-event") return "hardware:next-step-or-event";
        if (label == "prev-bar-start") return "hardware:prev-bar-or-start";
        if (label == "next-bar-end") return "hardware:next-bar-or-end";
        if (label == "tap") return "hardware:tap-or-note-repeat";
        if (label == "full-level") return "hardware:full-level-or-case-switch";
        if (label == "sixteen-levels") return "hardware:sixteen-levels-or-space";
        if (label == "enter") return "hardware:enter-or-save";
        if (label == "after") return "hardware:after-or-assign";
        if (label.substr(0, 1) == "0") return "hardware:0-or-vmpc";
        if (label.substr(0, 1) == "1") return "hardware:1-or-song";
        if (label.substr(0, 1) == "2") return "hardware:2-or-misc";
        if (label.substr(0, 1) == "3") return "hardware:3-or-load";
        if (label.substr(0, 1) == "4") return "hardware:4-or-sample";
        if (label.substr(0, 1) == "5") return "hardware:5-or-trim";
        if (label.substr(0, 1) == "6") return "hardware:6-or-program";
        if (label.substr(0, 1) == "7") return "hardware:7-or-mixer";
        if (label.substr(0, 1) == "8") return "hardware:8-or-other";
        if (label.substr(0, 1) == "9") return "hardware:9-or-midi-sync";

        return "hardware:" + label;
    }
} // namespace mpc::input::midi::legacy