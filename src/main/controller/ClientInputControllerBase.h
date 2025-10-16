#pragma once

#include "inputlogic/HostInputEvent.h"
#include "inputlogic/HostToClientTranslator.h"
#include "inputlogic/ClientInput.h"

#include "controller/ButtonLockTracker.h"

#include "mpc_fs.hpp"

#include "controls/KbMapping.hpp"
#include <cassert>

namespace mpc::controller {

class ClientInputControllerBase {
    public:
        struct PhysicalPadPress {
            int bankIndex;
            std::string screenName;
        };

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

        void registerPhysicalPadPush(const int padIndex, const int bankIndex, const std::string screenName)
        {
            assert(physicalPadPresses.count(padIndex) == 0);
            physicalPadPresses[padIndex] = { bankIndex, screenName };
        }

        PhysicalPadPress registerPhysicalPadRelease(const int padIndex)
        {
            assert(physicalPadPresses.count(padIndex) == 1);
            const auto result = physicalPadPresses[padIndex];
            physicalPadPresses.erase(padIndex);
            return result;
        }

    private:
        std::shared_ptr<mpc::controls::KbMapping> kbMapping;
        //
        // Maps physical pad index 0 - 15 to info at the time the pad was pushed.
        std::unordered_map<int, PhysicalPadPress> physicalPadPresses;

};

} // namespace mpc::controller
