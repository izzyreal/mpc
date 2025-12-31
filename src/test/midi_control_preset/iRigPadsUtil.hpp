#pragma once

#include <nlohmann/json.hpp>

using nlohmann::json;

inline void checkIRigPadsPreset(const json &preset)
{
    if (preset["name"] != "iRig PADS")
    {
        throw std::runtime_error("Preset name is not 'iRig PADS'");
    }
    if (preset["midiControllerDeviceName"] != "iRig PADS")
    {
        throw std::runtime_error("Device name is not 'iRig PADS'");
    }

    struct BindingSpec
    {
        std::string type;
        int number;
        int value;
        int channel;
        bool enabled = true;
    };

    const std::unordered_map<std::string, BindingSpec> expected = {
        {"hardware:pad-1-or-ab", {"Note", 35, 0, -1}},
        {"hardware:pad-2-or-cd", {"Note", 36, 0, -1}},
        {"hardware:pad-3-or-ef", {"Note", 38, 0, -1}},
        {"hardware:pad-4-or-gh", {"Note", 40, 0, -1}},
        {"hardware:pad-5-or-ij", {"Note", 37, 0, -1}},
        {"hardware:pad-6-or-kl", {"Note", 39, 0, -1}},
        {"hardware:pad-7-or-mn", {"Note", 42, 0, -1}},
        {"hardware:pad-8-or-op", {"Note", 44, 0, -1}},
        {"hardware:pad-9-or-qr", {"Note", 50, 0, -1}},
        {"hardware:pad-10-or-st", {"Note", 45, 0, -1}},
        {"hardware:pad-11-or-uv", {"Note", 41, 0, -1}},
        {"hardware:pad-12-or-wx", {"Note", 46, 0, -1}},
        {"hardware:pad-13-or-yz", {"Note", 51, 0, -1}},
        {"hardware:pad-14-or-ampersand-octothorpe", {"Note", 53, 0, -1}},
        {"hardware:pad-15-or-hyphen-eclamation-mark",
         {"Note", 49, 0, -1}},
        {"hardware:pad-16-or-parentheses", {"Note", 52, 0, -1}},
        {"hardware:data-wheel:negative", {"CC", 7, -1, -1}},
        {"hardware:data-wheel:positive", {"CC", 7, -1, -1}},
        {"hardware:data-wheel:negative", {"CC", 7, -1, -1, false}},
        {"hardware:data-wheel:positive", {"CC", 7, -1, -1, false}},
        {"hardware:slider", {"CC", 1, -1, -1}},
        {"hardware:rec-gain", {"CC", 11, -1, -1}},
        {"hardware:main-volume", {"CC", 10, -1, -1}},
        {"hardware:full-level-or-case-switch", {"CC", 20, -1, -1}},
        {"hardware:sixteen-levels-or-space", {"CC", 21, -1, -1}},
    };

    std::vector<std::pair<std::string, json>> actual;

    for (const auto &binding : preset["bindings"])
    {
        actual.emplace_back(binding["target"].get<std::string>(), binding);
    }

    for (const auto &kv : expected)
    {
        const std::string &target = kv.first;
        const BindingSpec &spec = kv.second;
        bool found = false;

        for (auto &p : actual)
        {
            if (p.first != target) continue;
            const json &b = p.second;
            if (b["enabled"].get<bool>() != spec.enabled) continue;
            if (b["messageType"] != spec.type) continue;
            if (b["midiNumber"].get<int>() != spec.number) continue;
            if (b["midiChannelIndex"].get<int>() != spec.channel) continue;
            if (spec.type == "CC")
            {
                if (!b.contains("midiValue")) continue;
                int actualVal = b["midiValue"].get<int>();
                if (spec.value == -1 && actualVal != -1) continue;
            }
            else
            {
                if (b.contains("midiValue")) continue;
            }
            found = true;
            break;
        }

        if (!found)
        {
            throw std::runtime_error("No matching binding for: " + target);
        }
    }

    for (const auto &binding : preset["bindings"])
    {
        std::string target = binding["target"].get<std::string>();
        if (!expected.count(target))
        {
            if (binding["enabled"].get<bool>())
            {
                throw std::runtime_error("Unexpected binding enabled: " +
                                         target);
            }
        }
    }
}
