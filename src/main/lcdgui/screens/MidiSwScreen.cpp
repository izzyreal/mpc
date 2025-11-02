#include "MidiSwScreen.hpp"
#include "VmpcSettingsScreen.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "lcdgui/screens/window/VmpcWarningSettingsIgnoredScreen.hpp"

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

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();

    if (ls->isPreviousScreenNot<VmpcWarningSettingsIgnoredScreen>() &&
        vmpcSettingsScreen->midiControlMode ==
            VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        ls->Draw();
        mpc.getLayeredScreen()->openScreen<VmpcWarningSettingsIgnoredScreen>();
    }

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    const auto &bindings = footswitchController->bindings;

    printf("bindings size: %zu\n", bindings.size());
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

        std::visit(
            [&](auto &b)
            {
                ctrlText = b.number == -1 ? "OFF" : std::to_string(b.number);

                // Reverse-lookup the enum to get the display name
                if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                             HardwareBinding>)
                {
                    for (const auto &[fn, cid] : footswitchToComponentId)
                    {
                        if (cid == b.target.componentId)
                        {
                            fnText = functionNameFromEnum.at(fn);
                            break;
                        }
                    }
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                                  SequencerBinding>)
                {
                    for (const auto &[fn, cmd] : footswitchToSequencerCmd)
                    {
                        if (cmd == b.target.command)
                        {
                            fnText = functionNameFromEnum.at(fn);
                            break;
                        }
                    }
                }
            },
            bindings[idx]);

        ctrlField->setText(ctrlText);
        functionField->setText(fnText.empty() ? "-" : fnText);
    }
}

void MidiSwScreen::turnWheel(int delta)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const int xPos =
        stoi(focusedFieldName.substr(focusedFieldName.length() - 1));
    const int selectedSwitch = xOffset + xPos;
    const bool editingCtrl = focusedFieldName.rfind("ctrl", 0) == 0;

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    if (selectedSwitch < 0 ||
        selectedSwitch >=
            static_cast<int>(footswitchController->bindings.size()))
    {
        return;
    }

    auto &binding = footswitchController->bindings[selectedSwitch];

    if (editingCtrl)
    {
        std::visit(
            [&](auto &b)
            {
                b.number = std::clamp(b.number + delta, -1, 127);
            },
            binding);
    }
    else
    {
        // Unified ordered list of all possible functions
        std::vector<MidiFootswitchFunction> allFns;
        for (int i = 0; i <= static_cast<int>(MidiFootswitchFunction::F6); ++i)
        {
            allFns.push_back(static_cast<MidiFootswitchFunction>(i));
        }

        // Find current index
        int currentIdx = 0;
        std::visit(
            [&](auto &b)
            {
                using T = std::decay_t<decltype(b)>;
                if constexpr (std::is_same_v<T,
                                             mpc::midi::input::HardwareBinding>)
                {
                    for (auto &[fn, cid] : footswitchToComponentId)
                    {
                        if (cid == b.target.componentId)
                        {
                            currentIdx = static_cast<int>(fn);
                        }
                    }
                }
                else if constexpr (std::is_same_v<
                                       T, mpc::midi::input::SequencerBinding>)
                {
                    for (auto &[fn, cmd] : footswitchToSequencerCmd)
                    {
                        if (cmd == b.target.command)
                        {
                            currentIdx = static_cast<int>(fn);
                        }
                    }
                }
            },
            binding);

        // Compute new function index
        int newIdx = std::clamp(currentIdx + delta, 0, (int)allFns.size() - 1);
        MidiFootswitchFunction newFn = allFns[newIdx];

        // Replace binding with correct variant
        std::visit(
            [&](auto &b)
            {
                auto base = b; // copy shared fields

                if (footswitchToComponentId.count(newFn) > 0)
                {
                    HardwareBinding newB;
                    static_cast<MidiBindingBase &>(newB) = base;
                    newB.target.componentId = footswitchToComponentId.at(newFn);
                    binding = newB;
                }
                else if (footswitchToSequencerCmd.count(newFn) > 0)
                {
                    SequencerBinding newB;
                    static_cast<MidiBindingBase &>(newB) = base;
                    newB.target.command = footswitchToSequencerCmd.at(newFn);
                    binding = newB;
                }
            },
            binding);
    }

    displayCtrlsAndFunctions();
}

void MidiSwScreen::function(int i)
{
    if (i == 0)
    {
        mpc.getLayeredScreen()->openScreen<SyncScreen>();
    }
}

void MidiSwScreen::left()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const int xPos =
        stoi(focusedFieldName.substr(focusedFieldName.length() - 1));

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
    const int xPos =
        stoi(focusedFieldName.substr(focusedFieldName.length() - 1));

    if (xPos == 3 &&
        xOffset < ClientMidiFootswitchAssignmentController::SWITCH_COUNT - 4)
    {
        setXOffset(xOffset + 1);
        return;
    }

    ScreenComponent::right();
}

void MidiSwScreen::setXOffset(int i)
{
    if (i < 0 ||
        i > controller::ClientMidiFootswitchAssignmentController::SWITCH_COUNT -
                4)
    {
        return;
    }
    xOffset = i;
    displaySwitchLabels();
    displayCtrlsAndFunctions();
}
