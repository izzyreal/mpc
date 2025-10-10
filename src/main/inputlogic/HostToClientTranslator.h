#pragma once

#include "inputlogic/HostInputEvent.h"
#include "inputlogic/ClientInput.h"

#include <memory>

namespace mpc::controls {
    class KbMapping;
}

namespace mpc::inputlogic {

struct HostToClientTranslator {
    // Translate HostInputEvent -> ClientInput. If translation cannot produce a meaningful input,
    // returns a ClientInput with kind == Unknown.
    static ClientInput translate(const HostInputEvent& hostEvent, std::shared_ptr<mpc::controls::KbMapping>);
};

} // namespace mpc::inputlogic
