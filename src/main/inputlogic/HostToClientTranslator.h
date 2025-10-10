#pragma once

#include "inputlogic/HostInputEvent.h"
#include "inputlogic/ClientInput.h"

namespace mpc::inputlogic {

struct HostToClientTranslator {
    // Translate HostInputEvent -> ClientInput. If translation cannot produce a meaningful input,
    // returns a ClientInput with kind == Unknown.
    static ClientInput translate(const HostInputEvent& hostEvent);
};

} // namespace mpc::inputlogic