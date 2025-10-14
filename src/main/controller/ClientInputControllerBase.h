#pragma once

#include "inputlogic/HostInputEvent.h"
#include "inputlogic/HostToClientTranslator.h"
#include "inputlogic/ClientInput.h"

#include "controller/ButtonLockTracker.h"

#include "mpc_fs.hpp"

#include "controls/KbMapping.hpp"

namespace mpc::controller {

class ClientInputControllerBase {
    public:
        explicit ClientInputControllerBase(const fs::path keyboardMappingConfigDirectory)
        {
            kbMapping = std::make_shared<mpc::controls::KbMapping>(keyboardMappingConfigDirectory);
        }

        void dispatchHostInput(const mpc::inputlogic::HostInputEvent& hostEvent)
        {
            const auto clientInput = mpc::inputlogic::HostToClientTranslator::translate(hostEvent, kbMapping);

            if (!clientInput.has_value())
            {
                printf("empty ClientInput\n");
                return;
            }

            handleAction(*clientInput);
        }


        virtual ~ClientInputControllerBase() = default;
        virtual void handleAction(const mpc::inputlogic::ClientInput& action) = 0;

        ButtonLockTracker buttonLockTracker;

        bool isNoteRepeatLocked() const { return buttonLockTracker.isLocked("note-repeat"); }
        void lockNoteRepeat() { buttonLockTracker.lock("note-repeat"); }
        void unlockNoteRepeat() { buttonLockTracker.unlock("note-repeat"); }

        std::shared_ptr<mpc::controls::KbMapping> getKbMapping() { return kbMapping; }

    private:
        std::shared_ptr<mpc::controls::KbMapping> kbMapping;
};

} // namespace mpc::controller
