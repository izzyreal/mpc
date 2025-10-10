#pragma once
#include <string>
#include <optional>

namespace mpc::inputlogic {
struct ClientInputEvent {
    std::string component;
    std::string action;
    std::optional<int> value;
};
}