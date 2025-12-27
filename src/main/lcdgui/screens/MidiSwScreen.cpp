#include "MidiSwScreen.hpp"
#include "Mpc.hpp"
#include "VmpcSettingsScreen.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::controller;
using namespace mpc::input::midi;

MidiSwScreen::MidiSwScreen(Mpc &mpc, const int layerIndex)
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
        const auto label = findChild<TextComp>("switch" + std::to_string(i));
        label->setText("Switch " + std::to_string(i + 1));
    }
}

void MidiSwScreen::displayCtrlsAndFunctions()
{
    const auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    const auto &bindings = footswitchController->bindings;

    for (int i = 0; i < 4; i++)
    {
        const auto ctrlField = findChild<Field>("ctrl" + std::to_string(i));
        const auto functionField =
            findChild<Field>("function" + std::to_string(i));

        if (i >= static_cast<int>(bindings.size()))
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

                if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                             HardwareBinding>)
                {
                    if (auto fn = componentIdToFootswitch(b.target.componentId))
                    {
                        fnText = functionNameFromEnum.at(*fn);
                    }
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                                  SequencerBinding>)
                {
                    if (auto fn = sequencerCmdToFootswitch(b.target.command))
                    {
                        fnText = functionNameFromEnum.at(*fn);
                    }
                }
            },
            bindings[i]);

        ctrlField->setText(ctrlText);
        functionField->setText(fnText.empty() ? "-" : fnText);
    }
}

void MidiSwScreen::turnWheel(int delta)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const int xPos =
        std::stoi(focusedFieldName.substr(focusedFieldName.length() - 1));
    const bool editingCtrl = focusedFieldName.rfind("ctrl", 0) == 0;

    const auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    auto &binding = footswitchController->bindings[xPos];

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
        const auto &allFns = getAllFootswitchFunctions();

        int currentIdx = 0;
        std::visit(
            [&](auto &b)
            {
                if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                             HardwareBinding>)
                {
                    if (auto fn = componentIdToFootswitch(b.target.componentId))
                    {
                        currentIdx = static_cast<int>(*fn);
                    }
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(b)>,
                                                  SequencerBinding>)
                {
                    if (auto fn = sequencerCmdToFootswitch(b.target.command))
                    {
                        currentIdx = static_cast<int>(*fn);
                    }
                }
            },
            binding);

        const int newIdx = std::clamp(currentIdx + delta, 0,
                                      static_cast<int>(allFns.size()) - 1);
        const MidiFootswitchFunction newFn = allFns[newIdx];

        std::visit(
            [&](const auto &b)
            {
                auto base = b;

                if (const auto cid = footswitchToComponentIdOpt(newFn))
                {
                    HardwareBinding newB;
                    static_cast<MidiBindingBase &>(newB) = base;
                    newB.target.componentId = *cid;
                    binding = newB;
                }
                else if (const auto cmd = footswitchToSequencerCmdOpt(newFn))
                {
                    SequencerBinding newB;
                    static_cast<MidiBindingBase &>(newB) = base;
                    newB.target.command = *cmd;
                    binding = newB;
                }
            },
            binding);
    }

    displayCtrlsAndFunctions();
}

void MidiSwScreen::function(const int i)
{
    if (i == 0)
    {
        openScreenById(ScreenId::SyncScreen);
    }
}
