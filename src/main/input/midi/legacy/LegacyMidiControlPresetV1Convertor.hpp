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
    static json parseLegacyMidiControlPresetV1(const std::string &data)
    {
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

        json result;

        if (data.size() < 17)
        {
            throw std::runtime_error("Data too short for legacy V1 header");
        }

        unsigned char autoLoadModeByte = static_cast<unsigned char>(data[0]);
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

        std::string name = data.substr(1, 16);

        name.erase(name.find_last_not_of(' ') + 1);
        result[nameKey] = StrUtil::replaceAll(name, '_', " ");
        result[midiControllerDeviceNameKey] =
            StrUtil::replaceAll(name, '_', " ");

        result[versionKey] = CURRENT_PRESET_VERSION;

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
            const auto bestGuessTarget = mapLegacyLabelToHardwareTarget(label);

            json binding;
            binding[targetKey] = bestGuessTarget;
            binding[messageTypeKey] = typeByte == 0 ? controllerStr : noteStr;

            int midiChannel = static_cast<signed char>(channelByte);

            if (midiChannel > 15)
            {
                // The file is bound to be corrupted from here on.
                break;
            }

            binding[midiChannelIndexKey] = midiChannel;

            int midiNumber = static_cast<signed char>(numberByte);

            binding[midiNumberKey] = midiNumber;

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

        // Reconstruct data wheel behavior into our new model:
        // - either two directional bindings (negative/positive)
        // - or two clones of the single "datawheel" binding as +/-.
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

        // Now enforce the new schema rules for midiValue / encoderMode.
        // This legacy format never emits plain "hardware:data-wheel", only
        // the +/- variants, so we don't set encoderMode here at all.
        for (auto &binding : bindings)
        {
            const std::string target = binding.at(targetKey).get<std::string>();
            const std::string messageType =
                binding.at(messageTypeKey).get<std::string>();

            const bool isController = messageType == controllerStr;

            const bool isHardware = target.rfind("hardware:", 0) == 0;

            const bool isPad = target.rfind("hardware:pad-", 0) == 0;

            const bool isPot = target == "hardware:slider" ||
                               target == "hardware:rec-gain-pot" ||
                               target == "hardware:main-volume-pot";

            const bool isDataWheelPlain = target == "hardware:data-wheel";

            const bool isButtonLike =
                isHardware && !isPad && !isPot && !isDataWheelPlain;

            if (isController)
            {
                if (isButtonLike)
                {
                    // Button-like hardware and sequencer CC bindings
                    // require midiValue. Use a sensible default threshold.
                    if (!binding.contains(midiValueKey))
                    {
                        binding[midiValueKey] = 64;
                    }
                }
                else
                {
                    // Pads, pots, plain data wheel (if it ever appeared)
                    // must not have midiValue.
                    if (binding.contains(midiValueKey))
                    {
                        binding.erase(midiValueKey);
                    }
                }
            }
            else
            {
                // messageType == "note": midiValue is always forbidden.
                if (binding.contains(midiValueKey))
                {
                    binding.erase(midiValueKey);
                }
            }
        }

        result["bindings"] = bindings;

        return result;
    }
} // namespace mpc::input::midi::legacy
