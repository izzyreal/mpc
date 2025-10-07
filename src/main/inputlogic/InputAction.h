#pragma once

#include <string>
#include <optional>

namespace mpc::inputlogic {
struct InputAction {
    std::string id;
    std::optional<int> value; // e.g. velocity, aftertouch, slider pos
};
}
