#pragma once

#include "input/HostInputEvent.h"
#include "input/ClientInput.h"

#include <memory>

namespace mpc::input {

    class KeyboardBindings;

    struct HostToClientTranslator {
        static std::optional<ClientInput> translate(const HostInputEvent&, std::shared_ptr<KeyboardBindings>);
    };

} // namespace mpc::input
