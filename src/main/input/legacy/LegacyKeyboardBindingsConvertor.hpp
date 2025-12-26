#pragma once

#include "input/KeyboardBindings.hpp"

#include "StrUtil.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace mpc::input::legacy
{
    class LegacyKeyboardBindingsConvertor
    {
    public:
        static KeyboardBindingsData
        parseLegacyKeyboardBindings(const std::string &data)
        {
            using Id = hardware::ComponentId;

            static const std::string magic = "vmpc2000xl_keyboard_mapping\nv1";

            KeyboardBindingsData kbBindingsData;

            if (data.substr(0, magic.length()) != magic)
            {
                return kbBindingsData;
            }

            size_t pos = magic.length() + 1;

            while (pos < data.size())
            {
                std::string labelName;

                while (pos < data.size() && data[pos] != ' ')
                {
                    labelName.push_back(data[pos++]);
                }

                pos++;

                std::string vmpcKeyCodeStr;

                while (pos < data.size() && data[pos] != '\n')
                {
                    vmpcKeyCodeStr.push_back(data[pos++]);
                }

                vmpcKeyCodeStr = StrUtil::trim(vmpcKeyCodeStr);

                auto vmpcKeyCode = VmpcKeyCode::VMPC_KEY_UNKNOWN;

                try
                {
                    if (int vmpcKeyCodeInt = stoi(vmpcKeyCodeStr);
                        vmpcKeyCodeInt >= 0 &&
                        vmpcKeyCodeInt <
                            static_cast<int>(
                                VmpcKeyCode::VMPC_KEY_ENUMERATOR_SIZE))
                    {
                        vmpcKeyCode = static_cast<VmpcKeyCode>(vmpcKeyCodeInt);
                    }
                }
                catch (const std::exception &)
                {
                }

                if (vmpcKeyCodeStr.empty() ||
                    vmpcKeyCode == VmpcKeyCode::VMPC_KEY_UNKNOWN)
                {
                    pos++;
                    continue;
                }

                if (labelName.find("extra") != std::string::npos)
                {
                    labelName = labelName.substr(0, 1);
                }

                Direction direction = Direction::NoDirection;

                if (labelName == "datawheel-down")
                {
                    direction = Direction::Negative;
                    labelName = hardware::componentIdToLabel.at(Id::DATA_WHEEL);
                }
                else if (labelName == "datawheel-up")
                {
                    direction = Direction::Positive;
                    labelName = hardware::componentIdToLabel.at(Id::DATA_WHEEL);
                }
                else if (labelName.find("shift_#") != std::string::npos)
                {
                    labelName = hardware::componentIdToLabel.at(Id::SHIFT);
                }
                else if (labelName.find("pad-") != std::string::npos)
                {
                    for (auto &newLabels : hardware::componentLabelToId)
                    {
                        if (newLabels.first.find(labelName + "-") !=
                            std::string::npos)
                        {
                            labelName = newLabels.first;
                        }
                    }
                }
                else if (labelName == "left")
                {
                    labelName = hardware::componentIdToLabel.at(
                        Id::CURSOR_LEFT_OR_DIGIT);
                }
                else if (labelName == "right")
                {
                    labelName = hardware::componentIdToLabel.at(
                        Id::CURSOR_RIGHT_OR_DIGIT);
                }
                else if (labelName == "up")
                {
                    labelName = hardware::componentIdToLabel.at(Id::CURSOR_UP);
                }
                else if (labelName == "down")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::CURSOR_DOWN);
                }
                else if (labelName == "prev-step-event")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::PREV_STEP_OR_EVENT);
                }
                else if (labelName == "next-step-event")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NEXT_STEP_OR_EVENT);
                }
                else if (labelName == "prev-bar-start")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::PREV_BAR_OR_START);
                }
                else if (labelName == "next-bar-end")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NEXT_BAR_OR_END);
                }
                else if (labelName == "tap")
                {
                    labelName = hardware::componentIdToLabel.at(
                        Id::TAP_TEMPO_OR_NOTE_REPEAT);
                }
                else if (labelName == "full-level")
                {
                    labelName = hardware::componentIdToLabel.at(
                        Id::FULL_LEVEL_OR_CASE_SWITCH);
                }
                else if (labelName == "sixteen-levels")
                {
                    labelName = hardware::componentIdToLabel.at(
                        Id::SIXTEEN_LEVELS_OR_SPACE);
                }
                else if (labelName == "0")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_0_OR_VMPC);
                }
                else if (labelName == "1")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_1_OR_SONG);
                }
                else if (labelName == "2")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_2_OR_MISC);
                }
                else if (labelName == "3")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_3_OR_LOAD);
                }
                else if (labelName == "4")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_4_OR_SAMPLE);
                }
                else if (labelName == "5")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_5_OR_TRIM);
                }
                else if (labelName == "6")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_6_OR_PROGRAM);
                }
                else if (labelName == "7")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_7_OR_MIXER);
                }
                else if (labelName == "8")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_8_OR_OTHER);
                }
                else if (labelName == "9")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::NUM_9_OR_MIDI_SYNC);
                }
                else if (labelName == "enter")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::ENTER_OR_SAVE);
                }
                else if (labelName == "after")
                {
                    labelName =
                        hardware::componentIdToLabel.at(Id::AFTER_OR_ASSIGN);
                }

                const bool isValidLabel =
                    hardware::componentLabelToId.count(labelName) > 0;

                if (!isValidLabel)
                {
                    pos++;
                    continue;
                }

                kbBindingsData.push_back({vmpcKeyCode, labelName, direction});
                pos++;
            }

            return kbBindingsData;
        }
    };
} // namespace mpc::input::legacy