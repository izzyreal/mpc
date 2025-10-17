#pragma once

#include "hardware/ComponentId.h"
#include "inputlogic/HostInputEvent.h"
#include "inputlogic/HostToClientTranslator.h"
#include "inputlogic/ClientInput.h"

#include "controller/ButtonLockTracker.h"

#include "mpc_fs.hpp"

#include "inputlogic/KeyboardBindings.h"
#include <cassert>
#include <memory>

namespace mpc::controller {

class ClientInputControllerBase {
    public:
        struct PhysicalPadPress {
            int bankIndex;
            std::string screenName;
        };

        explicit ClientInputControllerBase(const fs::path keyboardMappingConfigDirectory)
            : keyboardBindings(std::make_shared<mpc::inputlogic::KeyboardBindings>())
        {
        }

        void dispatchHostInput(const mpc::inputlogic::HostInputEvent& hostEvent)
        {
            const auto clientInput = mpc::inputlogic::HostToClientTranslator::translate(hostEvent, keyboardBindings);

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

        bool isNoteRepeatLocked() const { return buttonLockTracker.isLocked(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT); }
        void lockNoteRepeat() { buttonLockTracker.lock(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT); }
        void unlockNoteRepeat() { buttonLockTracker.unlock(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT); }

        std::shared_ptr<mpc::inputlogic::KeyboardBindings> getKeyboardBindings() { return keyboardBindings; }

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

    protected:
        // Maps component label to accumulated delta for discretization
        std::unordered_map<hardware::ComponentId, float> deltaAccumulators;

    private:
        std::shared_ptr<mpc::inputlogic::KeyboardBindings> keyboardBindings;
        //
        // Maps physical pad index 0 - 15 to info at the time the pad was pushed.
        std::unordered_map<int, PhysicalPadPress> physicalPadPresses;
};

} // namespace mpc::controller
