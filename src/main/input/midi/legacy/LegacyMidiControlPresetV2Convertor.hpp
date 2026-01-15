#pragma once

#include "LegacyMidiControlPresetUtil.hpp"
#include "StrUtil.hpp"
#include "input/midi/MidiControlPresetV3.hpp"

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

        result[versionKey] = 0;

        unsigned char autoLoadModeByte = static_cast<unsigned char>(data[1]);
        std::string autoLoadModeStr;
        switch (autoLoadModeByte)
        {
            case 0:
                autoLoadModeStr = autoLoadModeToString(AutoLoadModeNo);
                break;
            case 1:
                autoLoadModeStr = autoLoadModeToString(AutoLoadModeAsk);
                break;
            case 2:
                autoLoadModeStr = autoLoadModeToString(AutoLoadModeYes);
                break;
            default:
                autoLoadModeStr = autoLoadModeToString(AutoLoadModeNo);
                break;
        }
        result[autoLoadModeKey] = autoLoadModeStr;

        std::string name = data.substr(2, 16);
        name.erase(name.find_last_not_of(' ') + 1);

        result[nameKey] = mpc::StrUtil::replaceAll(name, '_', " ");
        result[midiControllerDeviceNameKey] =
            mpc::StrUtil::replaceAll(name, '_', " ");

        struct LegacyDatawheelBinding
        {
            std::string label;
            json binding;
        };

        std::vector<LegacyDatawheelBinding> datawheelBindings;
        bool hasDatawheel = false;
        bool datawheelUpEnabled = false;
        bool datawheelDownEnabled = false;
        bool datawheelEnabled = false;

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

            if (pos + 4 > data.size())
            {
                throw std::runtime_error("Unexpected end of data in binding");
            }

            unsigned char typeByte = static_cast<unsigned char>(data[pos++]);
            unsigned char channelByte = static_cast<unsigned char>(data[pos++]);
            unsigned char numberByte = static_cast<unsigned char>(data[pos++]);
            unsigned char ccValueByte = static_cast<unsigned char>(data[pos++]);
            (void)ccValueByte; // consumed but intentionally ignored

            const auto bestGuessTarget = mapLegacyLabelToHardwareTarget(label);

            json binding;
            binding[targetKey] = bestGuessTarget;
            binding[messageTypeKey] = (typeByte == 0)
                                          ? input::midi::controllerStr
                                          : input::midi::noteStr;

            int midiChannel = static_cast<signed char>(channelByte);
            binding[midiChannelIndexKey] = midiChannel;

            if (midiChannel > 15)
            {
                // The file is bound to be corrupted from here on.
                break;
            }

            int midiNumber = static_cast<signed char>(numberByte);

            binding[midiNumberKey] = midiNumber;

            // Do NOT set midiValue here; we'll assign a fixed 64 later
            // for targets that require it in the new schema.

            if (label.find("extra") != std::string::npos && midiNumber == -1)
            {
                continue;
            }

            if (label.find("shift_#") != std::string::npos && midiNumber == -1)
            {
                continue;
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
                    datawheelUpEnabled = midiNumber != -1;
                }
                else if (label == "datawheel-down")
                {
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
                    b.binding[targetKey] = "hardware:data-wheel";
                    b.binding[encoderModeKey] = encoderModeToString(
                        BindingEncoderMode::RelativeStateful);
                    bindings.push_back(b.binding);
                }
            }
        }

        // Post-pass: enforce new midiValue rules exactly like V1:
        // - if controller + button-like/sequencer → midiValue = 64
        // - else → no midiValue
        for (auto &binding : bindings)
        {
            const std::string target = binding.at(targetKey).get<std::string>();
            const std::string messageType =
                binding.at(messageTypeKey).get<std::string>();

            const bool isController =
                (messageType == input::midi::controllerStr);

            const bool isHardware = target.rfind("hardware:", 0) == 0;

            const bool isPad = target.rfind("hardware:pad-", 0) == 0;

            const bool isPot = target == "hardware:slider" ||
                               target == "hardware:rec-gain-pot" ||
                               target == "hardware:main-volume-pot";

            const bool isDataWheelPlain = target == "hardware:data-wheel";

            const bool isButtonLike =
                (isHardware && !isPad && !isPot && !isDataWheelPlain);

            if (isController && isButtonLike)
            {
                binding[midiValueKey] = 64;
            }
            else
            {
                binding.erase(midiValueKey);
            }
        }

        result[bindingsKey] = bindings;
        return result;
    }
} // namespace mpc::input::midi::legacy
