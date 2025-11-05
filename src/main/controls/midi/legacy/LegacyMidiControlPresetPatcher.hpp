#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

namespace mpc::controls::midi::legacy {
// Helper: validate a single property against its schema definition
inline bool isValidProperty(const json &value, const json &propSchema)
{
    if (value.is_null())
        return false;

    // Type check
    if (propSchema.contains("type")) {
        std::string type = propSchema["type"].get<std::string>();
        if (type == "string" && !value.is_string()) return false;
        if (type == "integer" && !value.is_number_integer()) return false;
        if (type == "boolean" && !value.is_boolean()) return false;
    }

    // Enum check
    if (propSchema.contains("enum") && value.is_string()) {
        bool ok = false;
        for (const auto &e : propSchema["enum"]) {
            if (value == e) { ok = true; break; }
        }
        if (!ok) return false;
    }

    // Numeric min/max check
    if (value.is_number_integer()) {
        int v = value.get<int>();
        if (propSchema.contains("minimum") && v < propSchema["minimum"].get<int>()) return false;
        if (propSchema.contains("maximum") && v > propSchema["maximum"].get<int>()) return false;
    }

    // String length/pattern check
    if (value.is_string()) {
        const std::string s = value.get<std::string>();
        if (propSchema.contains("minLength") && s.size() < propSchema["minLength"].get<size_t>()) return false;
        if (propSchema.contains("maxLength") && s.size() > propSchema["maxLength"].get<size_t>()) return false;
        if (propSchema.contains("pattern")) {
            std::regex re(propSchema["pattern"].get<std::string>());
            if (!std::regex_match(s, re)) return false;
        }
    }

    return true;
}

inline void patchLegacyPreset(json &preset, const json &schemaJson)
{
    const json &bindingDef = schemaJson["$defs"]["binding"];
    std::unordered_set<std::string> allowedLabels;
    for (const auto &lbl : bindingDef["properties"]["labelName"]["enum"]) {
        allowedLabels.insert(lbl.get<std::string>());
    }

    // Default binding (may not include labelName)
    json defaultBinding = bindingDef["default"];
    for (const auto &req : bindingDef["required"]) {
        if (!defaultBinding.contains(req.get<std::string>())) {
            defaultBinding[req.get<std::string>()] = nullptr;
        }
    }

    // Regexes for normalization
    std::regex extraRegex(R"(^([0-9]).*extra.*$)");
    std::regex legacyExtraRegex(R"(^([1-9])\s*\(extra\)$)");

    // 1) Sanitize & normalize labelName: build a new bindings array that only
    //    contains bindings with a usable string labelName.
    json sanitized = json::array();
    for (auto &binding : preset["bindings"]) {
        if (!binding.contains("labelName") || !binding["labelName"].is_string()) {
            // skip completely corrupted/non-string labelName entries
            continue;
        }
        std::string label = binding["labelName"].get<std::string>();

        std::smatch m;
        if (std::regex_match(label, m, legacyExtraRegex)) {
            label = m[1].str() + "_extra";
        } else if (std::regex_match(label, m, extraRegex)) {
            label = m[1].str() + "_extra";
        }

        binding["labelName"] = label;
        sanitized.push_back(binding);
    }
    preset["bindings"] = sanitized;

    // 2) Index existing (sanitized) bindings by labelName
    std::unordered_map<std::string, json*> bindingMap;
    for (auto &binding : preset["bindings"]) {
        bindingMap[binding["labelName"].get<std::string>()] = &binding;
    }

    // 3) Add missing bindings (use default binding as base and set labelName explicitly)
    for (const auto &lbl : allowedLabels) {
        if (bindingMap.find(lbl) == bindingMap.end()) {
            json newBinding = defaultBinding;
            newBinding["labelName"] = lbl;
            preset["bindings"].push_back(newBinding);
        }
    }

    // 4) Repair properties for each binding -- but DO NOT attempt to repair labelName here.
    const json &props = bindingDef["properties"];
    for (auto &binding : preset["bindings"]) {
        for (auto it = props.begin(); it != props.end(); ++it) {
            const std::string propName = it.key();
            if (propName == "labelName") continue; // handled already

            const json &propSchema = it.value();

            if (!binding.contains(propName) || !isValidProperty(binding[propName], propSchema)) {
                // Prefer a non-null default from defaultBinding
                if (defaultBinding.contains(propName) && !defaultBinding[propName].is_null()) {
                    binding[propName] = defaultBinding[propName];
                } else if (propSchema.contains("default")) {
                    binding[propName] = propSchema["default"];
                } else {
                    // Safe typed fallback instead of null
                    if (propSchema.contains("type")) {
                        std::string t = propSchema["type"].get<std::string>();
                        if (t == "string") binding[propName] = std::string("");
                        else if (t == "integer") binding[propName] = 0;
                        else if (t == "boolean") binding[propName] = false;
                        else binding[propName] = nullptr;
                    } else {
                        binding[propName] = nullptr;
                    }
                }
            }
        }

        // conditional schema rule: Note must not have midiValue
        if (binding.contains("messageType") && binding["messageType"] == "Note") {
            binding.erase("midiValue");
        }
    }

    // 5) Filter down to allowed labels and deduplicate (safe now: labelName is string)
    json filtered = json::array();
    std::unordered_set<std::string> seen;
    for (auto &binding : preset["bindings"]) {
        if (!binding.contains("labelName") || !binding["labelName"].is_string()) continue;
        std::string lbl = binding["labelName"].get<std::string>();
        if (allowedLabels.count(lbl) && !seen.count(lbl)) {
            filtered.push_back(binding);
            seen.insert(lbl);
        }
    }
    preset["bindings"] = filtered;

    // 6) Patch preset name / midi controller device name
    if (preset.contains("name") && preset["name"].is_string()) {
        std::string patchedName = preset["name"].get<std::string>();
        std::replace(patchedName.begin(), patchedName.end(), '_', ' ');
        preset["name"] = patchedName;
        preset["midiControllerDeviceName"] = patchedName;
    }
}

} // namespace mpc::controls::midi
