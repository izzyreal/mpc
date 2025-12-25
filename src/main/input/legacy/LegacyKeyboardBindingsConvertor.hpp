#pragma once

#include "input/KeyboardBindings.hpp"

#include "StrUtil.hpp"

#include <nlohmann/json.hpp>
#include <limits>

using json = nlohmann::json;

namespace mpc::input::legacy
{
    class LegacyKeyboardBindingsConvertor
    {
    public:
        static KeyboardBindingsData
        parseLegacyKeyboardBindings(const std::string &data)
        {
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

                std::string platformKeyCodeStr;

                while (pos < data.size() && data[pos] != '\n')
                {
                    platformKeyCodeStr.push_back(data[pos++]);
                }

                platformKeyCodeStr = mpc::StrUtil::trim(platformKeyCodeStr);

                int platformKeyCode = std::numeric_limits<int>::max();

                try
                {
                    platformKeyCode = stoi(platformKeyCodeStr);
                }
                catch (const std::exception &)
                {
                }

                if (platformKeyCodeStr.empty() ||
                    platformKeyCode == std::numeric_limits<int>::max())
                {
                    continue;
                }

                Direction direction = Direction::NoDirection;

                if (labelName == "datawheel-down")
                {
                    direction = Direction::Negative;
                    labelName = "data-wheel";
                }
                else if (labelName == "datawheel-up")
                {
                    direction = Direction::Positive;
                    labelName = "data-wheel";
                }
                else if (labelName.find("extra") != std::string::npos)
                {
                    labelName = labelName.substr(0, 1);
                }
                else if (labelName.find("shift_#") != std::string::npos)
                {
                    labelName = "shift";
                }
                else if (labelName.find("pad-") != std::string::npos)
                {
                    for (auto &newLabels : hardware::componentLabelToId)
                    {
                        if (newLabels.first.find(labelName) !=
                            std::string::npos)
                        {
                            labelName = newLabels.first;
                        }
                    }
                }

                const bool isValidLabel =
                    hardware::componentLabelToId.count(labelName) > 0;

                if (!isValidLabel)
                {
                    continue;
                }

                const auto vmpcKeyCode =
                    KeyCodeHelper::getVmpcFromPlatformKeyCode(platformKeyCode);

                const bool isValidKeyCode =
                    vmpcKeyCode != VmpcKeyCode::VMPC_KEY_UNKNOWN;

                if (!isValidKeyCode)
                {
                    continue;
                }

                kbBindingsData[vmpcKeyCode] = {labelName, direction};
            }

            return kbBindingsData;
        }
    };
} // namespace mpc::input::legacy