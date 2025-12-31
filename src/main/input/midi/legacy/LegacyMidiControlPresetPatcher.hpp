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
        for (const auto &target : bindingDef["properties"]["target"]["enum"])
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

        std::regex extraRegex(R"(^([0-9]).*extra.*$)");
        std::regex legacyExtraRegex(R"(^([1-9])\s*\(extra\)$)");

        json sanitized = json::array();
        for (auto &binding : preset["bindings"])
        {
            if (!binding.contains("target") ||
                !binding["target"].is_string())
            {
                continue;
            }
            std::string target = binding["target"].get<std::string>();

            std::smatch m;
            if (std::regex_match(target, m, legacyExtraRegex))
            {
                target = m[1].str() + "_extra";
            }
            else if (std::regex_match(target, m, extraRegex))
            {
                target = m[1].str() + "_extra";
            }

            binding["target"] = target;
            sanitized.push_back(binding);
        }
        preset["bindings"] = sanitized;

        std::unordered_map<std::string, json *> bindingMap;
        for (auto &binding : preset["bindings"])
        {
            bindingMap[binding["target"].get<std::string>()] = &binding;
        }

        for (const auto &lbl : allowedTargets)
        {
            if (bindingMap.find(lbl) == bindingMap.end())
            {
                json newBinding = defaultBinding;
                newBinding["target"] = lbl;
                preset["bindings"].push_back(newBinding);
            }
        }

        const json &props = bindingDef["properties"];
        for (auto &binding : preset["bindings"])
        {
            for (auto it = props.begin(); it != props.end(); ++it)
            {
                const std::string propName = it.key();
                if (propName == "target")
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

            if (binding.contains("messageType") &&
                binding["messageType"] == "Note")
            {
                binding.erase("midiValue");
            }
        }

        json filtered = json::array();
        std::unordered_set<std::string> seen;
        for (auto &binding : preset["bindings"])
        {
            if (!binding.contains("target") ||
                !binding["target"].is_string())
            {
                continue;
            }
            std::string target = binding["target"].get<std::string>();
            if (allowedTargets.count(target) && !seen.count(target))
            {
                filtered.push_back(binding);
                seen.insert(target);
            }
        }
        preset["bindings"] = filtered;

        if (preset.contains("name") && preset["name"].is_string())
        {
            std::string patchedName = preset["name"].get<std::string>();
            std::replace(patchedName.begin(), patchedName.end(), '_', ' ');
            preset["name"] = patchedName;
            preset["midiControllerDeviceName"] = patchedName;
        }
    }

} // namespace mpc::input::midi::legacy
