#include "MidiSwScreen.hpp"
#include "VmpcSettingsScreen.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::controller;
using namespace mpc::midi::input;

MidiSwScreen::MidiSwScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "midi-sw", layerIndex)
{
}

void MidiSwScreen::open()
{
    displaySwitchLabels();
    displayCtrlsAndFunctions();
}

void MidiSwScreen::displaySwitchLabels()
{
    for (int i = 0; i < 4; i++)
    {
        auto label = findChild<TextComp>("switch" + std::to_string(i));
        label->setText("Switch " + std::to_string(i + 1 + xOffset));
    }
}

void MidiSwScreen::displayCtrlsAndFunctions()
{
    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    const auto &bindings = footswitchController->bindings;

    for (int i = 0; i < 4; i++)
    {
        const int idx = i + xOffset;
        auto ctrlField = findChild<Field>("ctrl" + std::to_string(i));
        auto functionField = findChild<Field>("function" + std::to_string(i));

        if (idx >= static_cast<int>(bindings.size()))
        {
            ctrlField->setText("OFF");
            functionField->setText("-");
            continue;
        }

        std::string ctrlText;
        std::string fnText;

        std::visit([&](auto &b) {
            ctrlText = b.number == -1 ? "OFF" : std::to_string(b.number);

            if constexpr (std::is_same_v<std::decay_t<decltype(b)>, HardwareBinding>)
            {
                if (auto fn = componentIdToFootswitch(b.target.componentId))
                    fnText = functionNameFromEnum.at(*fn);
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(b)>, SequencerBinding>)
            {
                if (auto fn = sequencerCmdToFootswitch(b.target.command))
                    fnText = functionNameFromEnum.at(*fn);
            }
        }, bindings[idx]);

        ctrlField->setText(ctrlText);
        functionField->setText(fnText.empty() ? "-" : fnText);
    }
}

void MidiSwScreen::turnWheel(int delta)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const int xPos = std::stoi(focusedFieldName.substr(focusedFieldName.length() - 1));
    const int selectedSwitch = xOffset + xPos;
    const bool editingCtrl = focusedFieldName.rfind("ctrl", 0) == 0;

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    if (selectedSwitch < 0 ||
        selectedSwitch >= static_cast<int>(footswitchController->bindings.size()))
    {
        return;
    }

    auto &binding = footswitchController->bindings[selectedSwitch];

    if (editingCtrl)
    {
        std::visit([&](auto &b) {
            b.number = std::clamp(b.number + delta, -1, 127);
        }, binding);
    }
    else
    {
        // Use centralized ordered list
        const auto& allFns = getAllFootswitchFunctions();

        // Find current function index
        int currentIdx = 0;
        std::visit([&](auto &b) {
            if constexpr (std::is_same_v<std::decay_t<decltype(b)>, HardwareBinding>)
            {
                if (auto fn = componentIdToFootswitch(b.target.componentId))
                    currentIdx = static_cast<int>(*fn);
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(b)>, SequencerBinding>)
            {
                if (auto fn = sequencerCmdToFootswitch(b.target.command))
                    currentIdx = static_cast<int>(*fn);
            }
        }, binding);

        // Compute new function
        int newIdx = std::clamp(currentIdx + delta, 0, static_cast<int>(allFns.size()) - 1);
        MidiFootswitchFunction newFn = allFns[newIdx];

        // Replace binding with new function
        std::visit([&](auto &b) {
            auto base = b; // copy shared fields

            if (auto cid = footswitchToComponentIdOpt(newFn))
            {
                HardwareBinding newB;
                static_cast<MidiBindingBase &>(newB) = base;
                newB.target.componentId = *cid;
                binding = newB;
            }
            else if (auto cmd = footswitchToSequencerCmdOpt(newFn))
            {
                SequencerBinding newB;
                static_cast<MidiBindingBase &>(newB) = base;
                newB.target.command = *cmd;
                binding = newB;
            }
        }, binding);
    }

    displayCtrlsAndFunctions();
}

void MidiSwScreen::function(int i)
{
    if (i == 0)
        mpc.getLayeredScreen()->openScreen<SyncScreen>();
}

void MidiSwScreen::left()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const int xPos = std::stoi(focusedFieldName.substr(focusedFieldName.length() - 1));

    if (xPos == 0 && xOffset > 0)
    {
        setXOffset(xOffset - 1);
        return;
    }

    ScreenComponent::left();
}

void MidiSwScreen::right()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const int xPos = std::stoi(focusedFieldName.substr(focusedFieldName.length() - 1));

    if (xPos == 3 && xOffset < ClientMidiFootswitchAssignmentController::SWITCH_COUNT - 4)
    {
        setXOffset(xOffset + 1);
        return;
    }

    ScreenComponent::right();
}

void MidiSwScreen::setXOffset(int i)
{
    if (i < 0 || i > ClientMidiFootswitchAssignmentController::SWITCH_COUNT - 4)
        return;

    xOffset = i;
    displaySwitchLabels();
    displayCtrlsAndFunctions();
}
