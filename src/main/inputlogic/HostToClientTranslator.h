#pragma once

#include "inputlogic/HostInputEvent.h"
#include "inputlogic/ClientInput.h"

#include <memory>

namespace mpc::controls {
    class KbMapping;
}

namespace mpc::inputlogic {

struct HostToClientTranslator {
    static std::optional<ClientInput> translate(const HostInputEvent& hostEvent, std::shared_ptr<mpc::controls::KbMapping>);
};

} // namespace mpc::inputlogic
