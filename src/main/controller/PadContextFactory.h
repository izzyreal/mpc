#pragma once

#include "controls/PadPushContext.h"
#include "controls/PadReleaseContext.h"

namespace mpc {
    class Mpc;
}

namespace mpc::controller {
    class PadContextFactory {
        public:
            static mpc::controls::PadPushContext buildPadPushContext(mpc::Mpc& mpc, const int padIndexWithBank, const int velocity, const std::string currentScreenName);
            static mpc::controls::PadReleaseContext buildPadReleaseContext(mpc::Mpc& mpc, const int padIndexWithBank, const std::string currentScreenName);
    };
} // namespace mpc::controller
