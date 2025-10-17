#pragma once

#include "inputlogic/HostInputEvent.h"
#include "inputlogic/ClientInput.h"

#include <memory>

namespace mpc::inputlogic {

    class KeyboardBindings;

    struct HostToClientTranslator {
        static std::optional<ClientInput> translate(const HostInputEvent&, std::shared_ptr<KeyboardBindings>);
    };

} // namespace mpc::inputlogic
