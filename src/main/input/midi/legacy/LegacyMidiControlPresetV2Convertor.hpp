#pragma once

#include "LegacyMidiControlPresetUtil.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace mpc::input::midi::legacy
{
    static json parseLegacyMidiControlPresetV2(const std::string &data)
    {
        json result;

        if (data.size() < 18)
        {
            throw std::runtime_error("Data too short for legacy V2 header");
        }

        unsigned char versionByte = static_cast<unsigned char>(data[0]);
        if (versionByte != 2)
        {
            throw std::runtime_error("Not a legacy V2 preset");
        }
        result["midiControllerDeviceName"] = "";
        result["version"] = 0;

        unsigned char autoLoadByte = static_cast<unsigned char>(data[1]);
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

        std::string name = data.substr(2, 16);
        name.erase(name.find_last_not_of(' ') + 1);
        result["name"] = name;

        result["midiControllerDeviceName"] = "";

        std::vector<json> bindings;
        size_t pos = 18;
        while (pos < data.size())
        {
            size_t end = data.find(' ', pos);
            if (end == std::string::npos)
            {
                throw std::runtime_error(
                    "Malformed binding: no space terminator at pos " +
                    std::to_string(pos));
            }
            std::string label = data.substr(pos, end - pos);

            pos = end + 1;

            if (pos + 3 > data.size())
            {
                throw std::runtime_error("Unexpected end of data in binding");
            }

            unsigned char typeByte = static_cast<unsigned char>(data[pos++]);
            unsigned char channelByte = static_cast<unsigned char>(data[pos++]);
            unsigned char numberByte = static_cast<unsigned char>(data[pos++]);
            unsigned char ccValueByte = static_cast<unsigned char>(data[pos++]);

            const auto bestGuessTarget = mapLegacyLabelToHardwareTarget(label);

            json binding;
            binding["target"] = bestGuessTarget;
            binding["messageType"] = typeByte == 0 ? "CC" : "Note";

            int midiChannel = static_cast<signed char>(channelByte);
            binding["midiChannelIndex"] = midiChannel;

            int midiValue = static_cast<signed char>(ccValueByte);
            binding["midiValue"] = midiValue;

            int midiNumber = static_cast<signed char>(numberByte);

            if (midiNumber == -1)
            {
                binding["enabled"] = false;
                binding["midiNumber"] = 0;
            }
            else
            {
                binding["enabled"] = true;
                binding["midiNumber"] = midiNumber;
            }

            if (bestGuessTarget == "hardware:datawheel")
            {
                binding["target"] = "hardware:data-wheel:negative";
                bindings.push_back(binding);
                binding["target"] = "hardware:data-wheel:positive";
                bindings.push_back(binding);
            }
            else
            {
                bindings.push_back(binding);
            }
        }

        result["bindings"] = bindings;
        return result;
    }
} // namespace mpc::input::midi::legacy
