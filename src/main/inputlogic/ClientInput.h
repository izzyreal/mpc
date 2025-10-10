#pragma once

#include <string>
#include <optional>

namespace mpc::inputlogic {

/* Unified client-side input object (merged ClientInputEvent + ClientInputAction) */
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
        ButtonRelease
    };

    Type type = Type::Unknown;
    // For pad-related events: 0..15 (or more if banks applied elsewhere)
    std::optional<int> index;
    // For button events: label string (e.g. "play", "stop", "rec")
    std::optional<std::string> label;
    // Optional numeric value (velocity, pressure, slider position, wheel steps...)
    std::optional<int> value;
};

} // namespace mpc::inputlogic
