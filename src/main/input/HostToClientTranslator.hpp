#pragma once

#include "input/HostInputEvent.hpp"
#include "input/ClientInput.hpp"

#include <memory>

namespace mpc::input {

    class KeyboardBindings;

    struct HostToClientTranslator {
        static std::optional<ClientInput> translate(const HostInputEvent&, std::shared_ptr<KeyboardBindings>);
    };

} // namespace mpc::input
