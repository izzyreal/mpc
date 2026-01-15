#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

namespace mpc::input::midi::legacy
{
    inline bool isValidProperty(const json &value, const json &propSchema)
    {
        if (value.is_null())
        {
            return false;
        }

        if (propSchema.contains("type"))
        {
            std::string type = propSchema["type"].get<std::string>();
            if (type == "string" && !value.is_string())
            {
                return false;
            }
            if (type == "integer" && !value.is_number_integer())
            {
                return false;
            }
            if (type == "boolean" && !value.is_boolean())
            {
                return false;
            }
        }

        if (propSchema.contains("enum") && value.is_string())
        {
            bool ok = false;
            for (const auto &e : propSchema["enum"])
            {
                if (value == e)
                {
                    ok = true;
                    break;
                }
            }
            if (!ok)
            {
                return false;
            }
        }

        if (value.is_number_integer())
        {
            int v = value.get<int>();
            if (propSchema.contains("minimum") &&
                v < propSchema["minimum"].get<int>())
            {
                return false;
            }
            if (propSchema.contains("maximum") &&
                v > propSchema["maximum"].get<int>())
            {
                return false;
            }
        }

        if (value.is_string())
        {
            const std::string s = value.get<std::string>();
            if (propSchema.contains("minLength") &&
                s.size() < propSchema["minLength"].get<size_t>())
            {
                return false;
            }
            if (propSchema.contains("maxLength") &&
                s.size() > propSchema["maxLength"].get<size_t>())
            {
                return false;
            }
            if (propSchema.contains("pattern"))
            {
                std::regex re(propSchema["pattern"].get<std::string>());
                if (!std::regex_match(s, re))
                {
                    return false;
                }
            }
        }

        return true;
    }

    inline void patchLegacyPreset(json &preset, const json &schemaJson)
    {
        const json &bindingDef = schemaJson["$defs"]["binding"];
        std::unordered_set<std::string> allowedTargets;
        for (const auto &target : bindingDef["properties"][targetKey]["enum"])
        {
            allowedTargets.insert(target.get<std::string>());
        }

        json defaultBinding = bindingDef["default"];
        for (const auto &req : bindingDef["required"])
        {
            if (!defaultBinding.contains(req.get<std::string>()))
            {
                defaultBinding[req.get<std::string>()] = nullptr;
            }
        }

        json sanitized = json::array();
        for (auto &binding : preset[bindingsKey])
        {
            if (!binding.contains(targetKey) || !binding[targetKey].is_string())
            {
                continue;
            }

            binding[targetKey] = binding[targetKey].get<std::string>();
            sanitized.push_back(binding);
        }

        preset[bindingsKey] = sanitized;

        std::unordered_map<std::string, json *> bindingMap;
        for (auto &binding : preset[bindingsKey])
        {
            bindingMap[binding[targetKey].get<std::string>()] = &binding;
        }

        // Add missing targets with default binding
        for (const auto &lbl : allowedTargets)
        {
            if (bindingMap.find(lbl) == bindingMap.end())
            {
                json newBinding = defaultBinding;
                newBinding[targetKey] = lbl;

                if (lbl != "hardware:data-wheel")
                {
                    newBinding.erase(encoderModeKey);
                }

                preset[bindingsKey].push_back(newBinding);
            }
        }

        const json &props = bindingDef["properties"];
        for (auto &binding : preset[bindingsKey])
        {
            const std::string target =
                binding.contains(targetKey) && binding[targetKey].is_string()
                    ? binding[targetKey].get<std::string>()
                    : std::string{};

            // Fill/repair all properties except target, encoderMode, midiValue
            for (auto it = props.begin(); it != props.end(); ++it)
            {
                const std::string propName = it.key();
                if (propName == targetKey || propName == encoderModeKey ||
                    propName == midiValueKey)
                {
                    continue;
                }

                const json &propSchema = it.value();

                if (!binding.contains(propName) ||
                    !isValidProperty(binding[propName], propSchema))
                {
                    if (defaultBinding.contains(propName) &&
                        !defaultBinding[propName].is_null())
                    {
                        binding[propName] = defaultBinding[propName];
                    }
                    else if (propSchema.contains("default"))
                    {
                        binding[propName] = propSchema["default"];
                    }
                    else
                    {
                        if (propSchema.contains("type"))
                        {
                            std::string t =
                                propSchema["type"].get<std::string>();
                            if (t == "string")
                            {
                                binding[propName] = std::string("");
                            }
                            else if (t == "integer")
                            {
                                binding[propName] = 0;
                            }
                            else if (t == "boolean")
                            {
                                binding[propName] = false;
                            }
                            else
                            {
                                binding[propName] = nullptr;
                            }
                        }
                        else
                        {
                            binding[propName] = nullptr;
                        }
                    }
                }
            }

            // encoderMode: only allowed for hardware:data-wheel
            if (target == "hardware:data-wheel")
            {
                const json &encSchema = props.at(encoderModeKey);
                if (!binding.contains(encoderModeKey) ||
                    !isValidProperty(binding[encoderModeKey], encSchema))
                {
                    if (defaultBinding.contains(encoderModeKey) &&
                        !defaultBinding[encoderModeKey].is_null())
                    {
                        binding[encoderModeKey] = defaultBinding[encoderModeKey];
                    }
                }
            }
            else
            {
                binding.erase(encoderModeKey);
            }

            // midiValue: follow schema semantics
            if (!binding.contains(messageTypeKey) ||
                !binding[messageTypeKey].is_string())
            {
                binding.erase(midiValueKey);
            }
            else
            {
                const std::string mt =
                    binding[messageTypeKey].get<std::string>();
                const bool isController = (mt == "controller");

                if (!isController)
                {
                    // Note bindings must not have midiValue
                    binding.erase(midiValueKey);
                }
                else
                {
                    const bool isPad = target.rfind("hardware:pad-", 0) == 0;

                    const bool isPot = target == "hardware:slider" ||
                                       target == "hardware:rec-gain-pot" ||
                                       target == "hardware:main-volume-pot";

                    const bool isPlainDataWheel =
                        target == "hardware:data-wheel";

                    const bool isHardware = target.rfind("hardware:", 0) == 0;

                    const bool isButtonLike =
                        (isHardware && !isPad && !isPot && !isPlainDataWheel);

                    if (isButtonLike)
                    {
                        // Require midiValue; if missing, use default
                        if (!binding.contains(midiValueKey) ||
                            !binding[midiValueKey].is_number_integer())
                        {
                            if (defaultBinding.contains(midiValueKey))
                            {
                                binding[midiValueKey] =
                                    defaultBinding[midiValueKey];
                            }
                            else
                            {
                                binding[midiValueKey] = 64;
                            }
                        }
                    }
                    else
                    {
                        // Pads, pots, plain data-wheel: must not have midiValue
                        binding.erase(midiValueKey);
                    }
                }
            }
        }

        // Filter duplicates and unknown targets
        json filtered = json::array();
        std::unordered_set<std::string> seen;
        for (auto &binding : preset[bindingsKey])
        {
            if (!binding.contains(targetKey) || !binding[targetKey].is_string())
            {
                continue;
            }
            std::string target = binding[targetKey].get<std::string>();
            if (allowedTargets.count(target) && !seen.count(target))
            {
                filtered.push_back(binding);
                seen.insert(target);
            }
        }
        preset[bindingsKey] = filtered;

        if (preset.contains(nameKey) && preset[nameKey].is_string())
        {
            std::string patchedName = preset[nameKey].get<std::string>();
            std::replace(patchedName.begin(), patchedName.end(), '_', ' ');
            preset[nameKey] = patchedName;
            preset[midiControllerDeviceNameKey] = patchedName;
        }
    }
} // namespace mpc::input::midi::legacy
