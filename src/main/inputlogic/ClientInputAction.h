#pragma once

#include <string>
#include <optional>


/* Input actions within the client, i.e. the virtualized MPC2000XL */
namespace mpc::inputlogic {
struct ClientInputAction {
    std::string id;
    std::optional<int> value; // e.g. velocity, aftertouch, slider pos
};
}
