#pragma once

#include <optional>

#include "hardware/HardwareComponent.h"

namespace mpc::input {

struct ClientInput {

    enum Source { HostInputMidi, HostInputGesture, HostInputKeyboard, HostFocusEvent, Internal };
    
    Source source;
    
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
        ButtonDoublePress,
        HostFocusLost,
        ButtonPressAndRelease
    };

    Type type = Type::Unknown;
    std::optional<int> index;
    hardware::ComponentId componentId;
    std::optional<float> value;
    std::optional<float> deltaValue;
};

} // namespace mpc::input
