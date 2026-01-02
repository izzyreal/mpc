#pragma once

#include "LegacyMidiControlPresetUtil.hpp"

#include "StrUtil.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace mpc::input::midi::legacy
{
    static json parseLegacyMidiControlPresetV1(const std::string &data)
    {
        struct LegacyDatawheelBinding
        {
            std::string label;
            json binding;
        };

        std::vector<LegacyDatawheelBinding> datawheelBindings;
        bool hasDatawheelUp = false;
        bool hasDatawheelDown = false;
        bool hasDatawheel = false;
        bool datawheelUpEnabled = false;
        bool datawheelDownEnabled = false;
        bool datawheelEnabled = false;

        json result;

        if (data.size() < 17)
        {
            throw std::runtime_error("Data too short for legacy V1 header");
        }

        unsigned char autoLoadByte = static_cast<unsigned char>(data[0]);
        std::string autoLoadStr;
        switch (autoLoadByte)
        {
            case 0:
                autoLoadStr = "No";
                break;
            case 1:
                autoLoadStr = "Ask";
                break;
            case 2:
                autoLoadStr = "Yes";
                break;
            default:
                autoLoadStr = "No";
                break;
        }
        result["autoLoad"] = autoLoadStr;

        std::string name = data.substr(1, 16);

        name.erase(name.find_last_not_of(' ') + 1);
        result["name"] = mpc::StrUtil::replaceAll(name, '_', " ");
        result["midiControllerDeviceName"] =
            mpc::StrUtil::replaceAll(name, '_', " ");
        result["version"] = 0;

        std::vector<json> bindings;

        size_t pos = 17;
        while (pos < data.size())
        {
            std::string label;
            bool isExtraLabel = false;
            if (pos + 10 <= data.size())
            {
                if (std::string potentialExtra = data.substr(pos, 9);
                    potentialExtra.size() == 9 && potentialExtra[0] >= '0' &&
                    potentialExtra[0] <= '9' &&
                    potentialExtra.substr(1, 8) == " (extra)")
                {
                    label = potentialExtra;
                    pos += 10;
                    isExtraLabel = true;
                }
            }

            if (!isExtraLabel)
            {
                size_t end = data.find(' ', pos);
                if (end == std::string::npos)
                {
                    throw std::runtime_error(
                        "Malformed binding: no space terminator");
                }
                label = data.substr(pos, end - pos);
                pos = end + 1;
            }

            if (pos + 3 > data.size())
            {
                throw std::runtime_error("Unexpected end of data in binding");
            }

            unsigned char typeByte = static_cast<unsigned char>(data[pos++]);
            unsigned char channelByte = static_cast<unsigned char>(data[pos++]);
            unsigned char numberByte = static_cast<unsigned char>(data[pos++]);

            //            printf("=================\n");
            //            printf("label: %s\n", label.c_str());
            //            printf("type: %i\n", typeByte);
            //            printf("channel: %i\n", channelByte);
            //            printf("number: %i\n", numberByte);

            const auto bestGuessTarget = mapLegacyLabelToHardwareTarget(label);

            json binding;
            binding["target"] = bestGuessTarget;
            binding["messageType"] = typeByte == 0 ? "CC" : "Note";

            int midiChannel = static_cast<signed char>(channelByte);
            binding["midiChannelIndex"] = midiChannel;

            int midiNumber = static_cast<signed char>(numberByte);
            if (midiNumber == -1)
            {
                binding["midiNumber"] = 0;
                binding["enabled"] = false;
            }
            else
            {
                binding["midiNumber"] = midiNumber;
                binding["enabled"] = true;
            }

            if (typeByte == 0)
            {
                binding["midiValue"] = -1;
            }

            if (label.substr(0, 9) == "datawheel")
            {
                datawheelBindings.emplace_back(
                    LegacyDatawheelBinding{label, binding});
                if (label == "datawheel")
                {
                    hasDatawheel = true;
                    datawheelEnabled = midiNumber != -1;
                }
                else if (label == "datawheel-up")
                {
                    hasDatawheelUp = true;
                    datawheelUpEnabled = midiNumber != -1;
                }
                else if (label == "datawheel-down")
                {
                    hasDatawheelDown = true;
                    datawheelDownEnabled = midiNumber != -1;
                }
                continue;
            }

            bindings.push_back(binding);
        }

        if (hasDatawheel && !datawheelEnabled && datawheelUpEnabled &&
            datawheelDownEnabled)
        {
            for (auto &b : datawheelBindings)
            {
                if (b.label == "datawheel-down" || b.label == "datawheel-up")
                {
                    bindings.push_back(b.binding);
                }
            }
        }
        else if (datawheelEnabled)
        {
            for (auto &b : datawheelBindings)
            {
                if (b.label == "datawheel")
                {
                    b.binding["target"] = "hardware:data-wheel:negative";
                    bindings.push_back(b.binding);
                    b.binding["target"] = "hardware:data-wheel:positive";
                    bindings.push_back(b.binding);
                }
            }
        }

        result["bindings"] = bindings;

        return result;
    }
} // namespace mpc::input::midi::legacy
