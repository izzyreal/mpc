#pragma once
#include "HostInputEvent.h"
#include "ClientInputEvent.h"

namespace mpc::inputlogic {

class HostToClientTranslator {
public:
    static ClientInputEvent translate(const HostInputEvent& hostEvent);
};

}