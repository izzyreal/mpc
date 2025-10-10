#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "controls/Controls.hpp"
#include "hardware2/HardwareComponent.h"
#include "hardware/PadAndButtonKeyboard.hpp"
#include "hardware/TopPanel.hpp"
#include "hardware/Led.hpp"
#include "inputlogic/ClientInput.h"
#include "inputlogic/HostToClientTranslator.h"
#include "inputlogic/HostInputEvent.h"

#include "Mpc.hpp"

namespace mpc::hardware2 {

class Hardware2 final {
private:
    mpc::Mpc &mpc;
    mpc::hardware::PadAndButtonKeyboard* padAndButtonKeyboard = nullptr;
    std::vector<std::shared_ptr<mpc::hardware::Led>> leds;
    std::shared_ptr<mpc::hardware::TopPanel> topPanel;

    std::vector<std::string> buttonLabels;
    std::vector<std::shared_ptr<Pad>> pads;
    std::unordered_map<std::string, std::shared_ptr<Button>> buttons;
    std::shared_ptr<DataWheel> dataWheel;
    std::shared_ptr<Slider> slider;
    std::shared_ptr<Pot> recPot;
    std::shared_ptr<Pot> volPot;

public:
    explicit Hardware2(mpc::Mpc& mpcToUse) : mpc(mpcToUse)
    {
        padAndButtonKeyboard = new mpc::hardware::PadAndButtonKeyboard(mpc);
        topPanel = std::make_shared<mpc::hardware::TopPanel>();

        buttonLabels = {
            "left", "right", "up", "down", "rec", "overdub", "stop", "play", "play-start", "main-screen",
            "prev-step-event", "next-step-event", "go-to", "prev-bar-start", "next-bar-end", "tap",
            "next-seq", "track-mute", "open-window", "full-level", "sixteen-levels",
            "f1", "f2", "f3", "f4", "f5", "f6", "shift", "shift_#1", "shift_#2", "shift_#3",
            "enter", "undo-seq", "erase", "after", "bank-a", "bank-b", "bank-c", "bank-d",
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "0_extra", "1_extra", "2_extra", "3_extra", "4_extra", "5_extra",
            "6_extra", "7_extra", "8_extra", "9_extra"
        };

        for (const auto& label : buttonLabels)
            buttons[label] = std::make_shared<Button>(mpc.inputMapper, label);

        for (int i = 0; i < 16; ++i)
            pads.push_back(std::make_shared<Pad>(i, mpc.inputMapper));

        const std::vector<std::string> ledLabels = {
            "full-level", "sixteen-levels", "next-seq", "track-mute",
            "pad-bank-a", "pad-bank-b", "pad-bank-c", "pad-bank-d",
            "after", "undo-seq", "rec", "overdub", "play"
        };

        for (const auto& l : ledLabels)
            leds.push_back(std::make_shared<mpc::hardware::Led>(l));

        dataWheel = std::make_shared<DataWheel>(mpc.inputMapper);
        recPot = std::make_shared<Pot>(mpc.inputMapper);
        volPot = std::make_shared<Pot>(mpc.inputMapper);
        slider = std::make_shared<Slider>(mpc.inputMapper);
    }

    void dispatchHostInput(const mpc::inputlogic::HostInputEvent& hostEvent)
    {
        const auto clientEvent = mpc::inputlogic::HostToClientTranslator::translate(hostEvent, mpc.getControls()->getKbMapping().lock());
        dispatchClientInput(clientEvent);
    }

    void dispatchClientInput(const std::optional<mpc::inputlogic::ClientInput> maybeClientInput)
    {
        if (!maybeClientInput.has_value())
        {
            printf("Hardware2::dispatchClientInput received an empty ClientInput");
            return;
        }

        using ClientInput = mpc::inputlogic::ClientInput;

        const ClientInput &input = *maybeClientInput;

        switch (input.type) {
        case ClientInput::Type::PadPress:
            if (input.index) {
                int index = *input.index;
                if (index >= 0 && index < static_cast<int>(pads.size())) {
                    auto pad = pads[index];
                    if (input.value) pad->pressWithVelocity(*input.value);
                    else pad->press();
                }
            }
            break;
        case ClientInput::Type::PadRelease:
            if (input.index) {
                int index = *input.index;
                if (index >= 0 && index < static_cast<int>(pads.size())) {
                    pads[index]->release();
                }
            }
            break;
        case ClientInput::Type::PadAftertouch:
            if (input.index && input.value) {
                int index = *input.index;
                if (index >= 0 && index < static_cast<int>(pads.size())) {
                    pads[index]->aftertouch(*input.value);
                }
            }
            break;
        case ClientInput::Type::ButtonPress:
            if (input.label) {
                auto btn = getButton(*input.label);
                if (btn) btn->press();
            }
            break;
        case ClientInput::Type::ButtonRelease:
            if (input.label) {
                auto btn = getButton(*input.label);
                if (btn) btn->release();
            }
            break;
        case ClientInput::Type::DataWheelTurn:
            printf("data wheel\n");
            if (input.value) {
                dataWheel->turn(*input.value);
            }
            break;
        case ClientInput::Type::SliderMove:
            printf("slider\n");
            if (input.value) slider->moveTo(*input.value);
            break;
        case ClientInput::Type::PotMove:
            break;
        default:
            break;
        }
    }

    mpc::hardware::PadAndButtonKeyboard* getPadAndButtonKeyboard() { return padAndButtonKeyboard; }
    std::shared_ptr<mpc::hardware::TopPanel> getTopPanel() { return topPanel; }
    std::vector<std::string>& getButtonLabels() { return buttonLabels; }

    std::shared_ptr<Pad> getPad(int index) {
        if (index < 0 || index >= static_cast<int>(pads.size())) return nullptr;
        return pads[index];
    }

    std::vector<std::shared_ptr<Pad>>& getPads() { return pads; }

    std::shared_ptr<Button> getButton(const std::string& label) {
        if (auto it = buttons.find(label); it != buttons.end())
            return it->second;
        return nullptr;
    }

    std::vector<std::shared_ptr<mpc::hardware::Led>> getLeds() { return leds; }
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

} // namespace mpc::hardware2
