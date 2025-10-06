#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

// Parses legacy MIDI preset binary data into JSON
static json parseLegacyMidiPresetV1(const std::string &data)
{
    json result;

    if (data.size() < 17) {
        throw std::runtime_error("Data too short for legacy V1 header");
    }

    // autoLoad is first byte
    unsigned char autoLoadByte = static_cast<unsigned char>(data[0]);
    std::string autoLoadStr;
    switch(autoLoadByte) {
        case 0: autoLoadStr = "No"; break;
        case 1: autoLoadStr = "Ask"; break;
        case 2: autoLoadStr = "Yes"; break;
        default: autoLoadStr = "No"; break;
    }
    result["autoLoad"] = autoLoadStr;

    // preset name is next 16 bytes
    std::string name = data.substr(1, 16);
    // trim trailing spaces
    name.erase(name.find_last_not_of(' ') + 1);
    result["name"] = name;

    result["midiControllerDeviceName"] = ""; // unknown in legacy format
    result["version"] = 0; // default

    // parse bindings
    std::vector<json> bindings;
    size_t pos = 17;
    while (pos < data.size()) {
        
        // Check for extra labels first
        std::string label;
        bool isExtraLabel = false;
        if (pos + 10 <= data.size()) {
            std::string potentialExtra = data.substr(pos, 9);
            if (potentialExtra.size() == 9 && 
                potentialExtra[0] >= '0' && potentialExtra[0] <= '9' &&
                potentialExtra.substr(1, 8) == " (extra)") {
                label = potentialExtra;
                pos += 10;
                isExtraLabel = true;
            }
        }

        // If not an extra label, use original space-terminated label logic
        if (!isExtraLabel) {
            size_t end = data.find(' ', pos);
            if (end == std::string::npos) {
                throw std::runtime_error("Malformed binding: no space terminator");
            }
            label = data.substr(pos, end - pos);
            pos = end + 1;
        }

        if (pos + 3 > data.size()) {
            throw std::runtime_error("Unexpected end of data in binding");
        }

        unsigned char typeByte = static_cast<unsigned char>(data[pos++]);
        unsigned char channelByte = static_cast<unsigned char>(data[pos++]);
        unsigned char numberByte = static_cast<unsigned char>(data[pos++]);

        json binding;
        binding["labelName"] = label;
        binding["messageType"] = (typeByte == 0) ? "CC" : "Note";

        int midiChannel = static_cast<signed char>(channelByte); // -1..15
        binding["midiChannelIndex"] = midiChannel;

        int midiNumber = static_cast<signed char>(numberByte);
        if (midiNumber == -1) {
            binding["enabled"] = false;
            binding["midiNumber"] = 0; // placeholder
            binding["midiValue"] = 0; // placeholder
        } else {
            binding["enabled"] = true;
            binding["midiNumber"] = midiNumber;
            // legacy format doesn't have CC value, behaviour was assumed to like "all", so we set it to -1
            binding["midiValue"] = -1;
        }

        bindings.push_back(binding);
    }

    result["bindings"] = bindings;

    return result;
}
