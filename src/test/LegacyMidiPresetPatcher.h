#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

using nlohmann::json;
using nlohmann::json_schema::json_validator;

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

// Patches a legacy-converted preset so that it conforms to the schema
inline void patchLegacyPreset(json &preset, const json &schemaJson)
{
    const json &bindingDef = schemaJson["$defs"]["binding"];
    std::unordered_set<std::string> allowedLabels;
    for (const auto &lbl : bindingDef["properties"]["labelName"]["enum"]) {
        allowedLabels.insert(lbl.get<std::string>());
    }

    // Default binding
    json defaultBinding = bindingDef["default"];
    for (const auto &req : bindingDef["required"]) {
        if (!defaultBinding.contains(req.get<std::string>())) {
            defaultBinding[req.get<std::string>()] = nullptr;
        }
    }

    std::regex extraRegex(R"(^([0-9]).*extra.*$)");

    // Normalize label names
    for (auto &binding : preset["bindings"]) {
        std::string label = binding["labelName"].get<std::string>();
        std::smatch match;
        if (std::regex_match(label, match, extraRegex)) {
            binding["labelName"] = match[1].str() + "_extra";
        }
    }

    // Index by labelName
    std::unordered_map<std::string, json*> bindingMap;
    for (auto &binding : preset["bindings"]) {
        bindingMap[binding["labelName"].get<std::string>()] = &binding;
    }

    // Add missing bindings
    for (const auto &lbl : allowedLabels) {
        if (bindingMap.find(lbl) == bindingMap.end()) {
            json newBinding = defaultBinding;
            newBinding["labelName"] = lbl;
            preset["bindings"].push_back(newBinding);
        }
    }

    // Validate each binding’s properties and repair invalid ones
    const json &props = bindingDef["properties"];
    for (auto &binding : preset["bindings"]) {
        for (auto it = props.begin(); it != props.end(); ++it) {
            const std::string propName = it.key();
            const json &propSchema = it.value();

            if (!binding.contains(propName) || !isValidProperty(binding[propName], propSchema)) {
                // Repair with default if available
                if (defaultBinding.contains(propName)) {
                    binding[propName] = defaultBinding[propName];
                } else {
                    // If schema has a "default" inside the property itself
                    if (propSchema.contains("default")) {
                        binding[propName] = propSchema["default"];
                    } else {
                        // fallback to null if absolutely nothing is specified
                        binding[propName] = nullptr;
                    }
                }
            }
        }
        // --- handle conditional rules ---
        if (binding["messageType"] == "Note") {
            // For "Note", midiValue must not exist
            binding.erase("midiValue");
        }/* else if (binding["messageType"] == "CC") {
            // For "CC", ensure midiValue exists
            if (!binding.contains("midiValue")) {
                if (defaultBinding.contains("midiValue"))
                    binding["midiValue"] = defaultBinding["midiValue"];
                else
                    binding["midiValue"] = 0;
            }
        } */      
    }

    // Filter down to exactly allowed labels, deduplicated
    json filtered = json::array();
    std::unordered_set<std::string> seen;
    for (auto &binding : preset["bindings"]) {
        std::string lbl = binding["labelName"].get<std::string>();
        if (allowedLabels.count(lbl) && !seen.count(lbl)) {
            filtered.push_back(binding);
            seen.insert(lbl);
        }
    }
    preset["bindings"] = filtered;

        // --- patch preset name ---
    if (preset.contains("name") && preset["name"].is_string()) {
        std::string patchedName = preset["name"].get<std::string>();
        std::replace(patchedName.begin(), patchedName.end(), '_', ' ');
        preset["name"] = patchedName;
        preset["midiControllerDeviceName"] = patchedName;
    }
}

inline void checkIRigPadsPreset(const json &preset)
{
    // Name and device name
    if (preset["name"] != "iRig PADS") {
        throw std::runtime_error("Preset name is not 'iRig PADS'");
    }
    if (preset["midiControllerDeviceName"] != "iRig PADS") {
        throw std::runtime_error("Device name is not 'iRig PADS'");
    }

    // Expected active bindings
    struct BindingSpec {
        std::string type;   // "Note" or "CC"
        int number;
        bool needsValue;
        int value;
        int channel; // -1 = all
    };

    std::unordered_map<std::string, BindingSpec> expected = {
        {"pad-1", {"Note", 35, false, 0, -1}},
        {"pad-2", {"Note", 36, false, 0, -1}},
        {"pad-3", {"Note", 38, false, 0, -1}},
        {"pad-4", {"Note", 40, false, 0, -1}},
        {"pad-5", {"Note", 37, false, 0, -1}},
        {"pad-6", {"Note", 39, false, 0, -1}},
        {"pad-7", {"Note", 42, false, 0, -1}},
        {"pad-8", {"Note", 44, false, 0, -1}},
        {"pad-9", {"Note", 50, false, 0, -1}},
        {"pad-10", {"Note", 45, false, 0, -1}},
        {"pad-11", {"Note", 41, false, 0, -1}},
        {"pad-12", {"Note", 46, false, 0, -1}},
        {"pad-13", {"Note", 51, false, 0, -1}},
        {"pad-14", {"Note", 53, false, 0, -1}},
        {"pad-15", {"Note", 49, false, 0, -1}},
        {"pad-16", {"Note", 52, false, 0, -1}},
        {"datawheel", {"CC", 7, true, -1, -1}},     // -1 means "all"
        {"slider", {"CC", 1, true, -1, -1}},
        {"rec-gain", {"CC", 11, true, -1, -1}},
        {"main-volume", {"CC", 10, true, -1, -1}},
        {"full-level", {"CC", 20, true, -1, -1}},
        {"sixteen-levels", {"CC", 21, true, -1, -1}},
    };

    // Build lookup of actual bindings
    std::unordered_map<std::string, json> actual;
    for (const auto &binding : preset["bindings"]) {
        actual[binding["labelName"].get<std::string>()] = binding;
    }

    // Check expected bindings
    for (const auto &kv : expected) {
        const std::string &label = kv.first;
        const BindingSpec &spec = kv.second;
        if (!actual.count(label)) {
            throw std::runtime_error("Missing binding: " + label);
        }
        const json &b = actual[label];
        if (!b["enabled"].get<bool>())
            throw std::runtime_error("Binding disabled: " + label);
        if (b["messageType"] != spec.type)
            throw std::runtime_error("Wrong type for " + label);
        if (b["midiNumber"].get<int>() != spec.number)
            throw std::runtime_error("Wrong midiNumber for " + label);
        if (b["midiChannelIndex"].get<int>() != spec.channel)
            throw std::runtime_error("Wrong channel for " + label);
        if (spec.type == "CC") {
            if (!b.contains("midiValue"))
                throw std::runtime_error("Missing midiValue for " + label);

            int actualVal = b["midiValue"].get<int>();
            if (spec.value == -1 && actualVal != -1) {
                throw std::runtime_error(
                    "Expected midiValue -1 (all) for " + label +
                    ", but got " + std::to_string(actualVal)
                );
            }
        } else {
            if (b.contains("midiValue")) {
                throw std::runtime_error(
                    "Note binding has forbidden midiValue: " + label +
                    " (found " + b["midiValue"].dump() + ")"
                );
            }
        }        
    }

    // Check all others are disabled
    for (const auto &binding : preset["bindings"]) {
        std::string lbl = binding["labelName"].get<std::string>();
        if (!expected.count(lbl)) {
            if (binding["enabled"].get<bool>()) {
                throw std::runtime_error("Unexpected binding enabled: " + lbl);
            }
        }
    }
}

