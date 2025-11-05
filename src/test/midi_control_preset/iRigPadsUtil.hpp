#include <nlohmann/json.hpp>

using nlohmann::json;

inline void checkIRigPadsPreset(const json &preset)
{
    // Name and device name
    if (preset["name"] != "iRig PADS")
    {
        throw std::runtime_error("Preset name is not 'iRig PADS'");
    }
    if (preset["midiControllerDeviceName"] != "iRig PADS")
    {
        throw std::runtime_error("Device name is not 'iRig PADS'");
    }

    // Expected active bindings
    struct BindingSpec
    {
        std::string type; // "Note" or "CC"
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
        {"datawheel", {"CC", 7, true, -1, -1}}, // -1 means "all"
        {"slider", {"CC", 1, true, -1, -1}},
        {"rec-gain", {"CC", 11, true, -1, -1}},
        {"main-volume", {"CC", 10, true, -1, -1}},
        {"full-level", {"CC", 20, true, -1, -1}},
        {"sixteen-levels", {"CC", 21, true, -1, -1}},
    };

    // Build lookup of actual bindings
    std::unordered_map<std::string, json> actual;
    for (const auto &binding : preset["bindings"])
    {
        actual[binding["labelName"].get<std::string>()] = binding;
    }

    // Check expected bindings
    for (const auto &kv : expected)
    {
        const std::string &label = kv.first;
        const BindingSpec &spec = kv.second;
        if (!actual.count(label))
        {
            throw std::runtime_error("Missing binding: " + label);
        }
        const json &b = actual[label];
        if (!b["enabled"].get<bool>())
        {
            throw std::runtime_error("Binding disabled: " + label);
        }
        if (b["messageType"] != spec.type)
        {
            throw std::runtime_error("Wrong type for " + label);
        }
        if (b["midiNumber"].get<int>() != spec.number)
        {
            throw std::runtime_error("Wrong midiNumber for " + label);
        }
        if (b["midiChannelIndex"].get<int>() != spec.channel)
        {
            throw std::runtime_error("Wrong channel for " + label);
        }
        if (spec.type == "CC")
        {
            if (!b.contains("midiValue"))
            {
                throw std::runtime_error("Missing midiValue for " + label);
            }

            int actualVal = b["midiValue"].get<int>();
            if (spec.value == -1 && actualVal != -1)
            {
                throw std::runtime_error("Expected midiValue -1 (all) for " +
                                         label + ", but got " +
                                         std::to_string(actualVal));
            }
        }
        else
        {
            if (b.contains("midiValue"))
            {
                throw std::runtime_error(
                    "Note binding has forbidden midiValue: " + label +
                    " (found " + b["midiValue"].dump() + ")");
            }
        }
    }

    // Check all others are disabled
    for (const auto &binding : preset["bindings"])
    {
        std::string lbl = binding["labelName"].get<std::string>();
        if (!expected.count(lbl))
        {
            if (binding["enabled"].get<bool>())
            {
                throw std::runtime_error("Unexpected binding enabled: " + lbl);
            }
        }
    }
}
