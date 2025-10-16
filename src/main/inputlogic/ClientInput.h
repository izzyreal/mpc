#pragma once

#include <string>
#include <optional>

namespace mpc::inputlogic {

struct ClientInput {
    enum class Type {
        Unknown,
        PadPress,
        PadRelease,
        PadAftertouch,
        DataWheelTurn,
        SliderMove,
        PotMove,
        ButtonPress,
        ButtonRelease,
        ButtonDoublePress
    };

    Type type = Type::Unknown;
    // For pad-related events: 0..15 (or more if banks applied elsewhere)
    std::optional<int> index;
    // For button events: label string (e.g. "play", "stop", "rec")
    std::optional<std::string> label;
    //

    std::optional<float> value;
    std::optional<float> deltaValue;
};

} // namespace mpc::inputlogic
