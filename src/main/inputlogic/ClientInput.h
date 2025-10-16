#pragma once

#include <string>
#include <optional>

#include "hardware/HardwareComponent.h"

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
    std::optional<int> index;
    hardware::ComponentId componentId;
    std::optional<float> value;
    std::optional<float> deltaValue;
};

} // namespace mpc::inputlogic
