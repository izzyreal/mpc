#pragma once

#include "command/context/TriggerDrumNoteOnContext.h"
#include "command/context/TriggerDrumNoteOffContext.h"

namespace mpc {
    class Mpc;
}

namespace mpc::command::context {
    class TriggerDrumContextFactory {
        public:
            static TriggerDrumNoteOnContext buildTriggerDrumNoteOnContext(mpc::Mpc& mpc, const int padIndexWithBank, const int velocity, const std::string currentScreenName);
            static TriggerDrumNoteOffContext buildTriggerDrumNoteOffContext(mpc::Mpc& mpc, const int padIndexWithBank, const std::string currentScreenName);
    };
} // namespace mpc::controller
