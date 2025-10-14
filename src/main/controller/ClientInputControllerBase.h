#pragma once

#include "inputlogic/ClientInput.h"

#include "controller/ButtonLockTracker.h"

namespace mpc::controller {

class ClientInputControllerBase {
    public:
        virtual ~ClientInputControllerBase() = default;
        virtual void handleAction(const mpc::inputlogic::ClientInput& action) = 0;

        ButtonLockTracker buttonLockTracker;

        bool isNoteRepeatLocked() const { return buttonLockTracker.isLocked("note-repeat"); }
        void lockNoteRepeat() { buttonLockTracker.lock("note-repeat"); }
        void unlockNoteRepeat() { buttonLockTracker.unlock("note-repeat"); }
};

} // namespace mpc::controller
