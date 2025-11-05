#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace mpc::controls::midi::legacy {
// Parses legacy MIDI preset V2 binary data into JSON
static json parseLegacyMidiControlPresetV2(const std::string &data)
{
    json result;

    if (data.size() < 18) {
        throw std::runtime_error("Data too short for legacy V2 header");
    }

    // First byte is version, must be 2
    unsigned char versionByte = static_cast<unsigned char>(data[0]);
    if (versionByte != 2) {
        throw std::runtime_error("Not a legacy V2 preset");
    }
    result["midiControllerDeviceName"] = ""; // unknown in legacy format
    result["version"] = 0; // default

    // Next byte is autoLoad
    unsigned char autoLoadByte = static_cast<unsigned char>(data[1]);
    std::string autoLoadStr;
    switch(autoLoadByte) {
        case 0: autoLoadStr = "No"; break;
        case 1: autoLoadStr = "Ask"; break;
        case 2: autoLoadStr = "Yes"; break;
        default: autoLoadStr = "No"; break;
    }
    result["autoLoad"] = autoLoadStr;

    // Preset name is next 16 bytes
    std::string name = data.substr(2, 16);
    name.erase(name.find_last_not_of(' ') + 1); // trim trailing spaces
    result["name"] = name;

    printf("preset name: %s\n", name.c_str());

    result["midiControllerDeviceName"] = ""; // unknown in legacy format

    // Parse bindings
    std::vector<json> bindings;
    size_t pos = 18;
    while (pos < data.size()) {
        // Read space-terminated label name
        size_t end = data.find(' ', pos);
        if (end == std::string::npos) {
            throw std::runtime_error("Malformed binding: no space terminator at pos " + std::to_string(pos));
        }
        std::string label = data.substr(pos, end - pos);

        pos = end + 1;

        if (pos + 3 > data.size()) {
            throw std::runtime_error("Unexpected end of data in binding");
        }

        unsigned char typeByte = static_cast<unsigned char>(data[pos++]);
        unsigned char channelByte = static_cast<unsigned char>(data[pos++]);
        unsigned char numberByte = static_cast<unsigned char>(data[pos++]);
        unsigned char ccValueByte = static_cast<unsigned char>(data[pos++]);

        json binding;
        binding["labelName"] = label;
        binding["messageType"] = (typeByte == 0) ? "CC" : "Note";

        int midiChannel = static_cast<signed char>(channelByte); // -1..15
        binding["midiChannelIndex"] = midiChannel;

        int midiValue = static_cast<signed char>(ccValueByte); // -1..127
        binding["midiValue"] = midiValue;

        int midiNumber = static_cast<signed char>(numberByte);

        if (midiNumber == -1) {
            binding["enabled"] = false;
            binding["midiNumber"] = 0;
        } else {
            binding["enabled"] = true;
            binding["midiNumber"] = midiNumber;
        }

        bindings.push_back(binding);
    }

    result["bindings"] = bindings;
    return result;
}
}
