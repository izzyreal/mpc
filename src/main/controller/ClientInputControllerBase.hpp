#pragma once

#include "hardware/ComponentId.hpp"
#include "input/HostInputEvent.hpp"
#include "input/HostToClientTranslator.hpp"
#include "input/ClientInput.hpp"

#include "controller/ButtonLockTracker.hpp"

#include "mpc_fs.hpp"

#include "input/KeyboardBindings.hpp"

#include "lcdgui/ScreenComponent.hpp"

#include <cassert>
#include <memory>

namespace mpc::sampler { class Program; }

namespace mpc::controller {

class ClientInputControllerBase {
    public:
        struct PhysicalPadPress {
            int bankIndex;
            std::shared_ptr<sampler::Program> program;
            std::shared_ptr<lcdgui::ScreenComponent> screen;
            input::ClientInput::Source inputSource;
        };

        explicit ClientInputControllerBase(const fs::path keyboardMappingConfigDirectory)
            : keyboardBindings(std::make_shared<mpc::input::KeyboardBindings>())
        {
        }

        void dispatchHostInput(const mpc::input::HostInputEvent& hostEvent)
        {
            const auto clientInput = mpc::input::HostToClientTranslator::translate(hostEvent, keyboardBindings);

            if (!clientInput.has_value())
            {
                printf("empty ClientInput\n");
                return;
            }

            handleInput(*clientInput);
        }


        virtual ~ClientInputControllerBase() = default;
        virtual void handleInput(const mpc::input::ClientInput&) = 0;

        ButtonLockTracker buttonLockTracker;

        bool isNoteRepeatLocked() const { return buttonLockTracker.isLocked(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT); }
        void lockNoteRepeat() { buttonLockTracker.lock(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT); }
        void unlockNoteRepeat() { buttonLockTracker.unlock(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT); }

        std::shared_ptr<mpc::input::KeyboardBindings> getKeyboardBindings() { return keyboardBindings; }

        bool isPhysicallyPressed(const int physicalPadIndex, const int bankIndex) const
        {
            return physicalPadPresses.count(physicalPadIndex) > 0 &&
                physicalPadPresses.at(physicalPadIndex).bankIndex == bankIndex;
        }

        bool isPhysicallyPressedDueToKeyboardInput(const int physicalPadIndex) const
        {
            if (physicalPadPresses.count(physicalPadIndex) == 0)
            {
                return false;
            }

            const auto pressSource = physicalPadPresses.at(physicalPadIndex).inputSource;

            return pressSource == input::ClientInput::Source::HostInputKeyboard;
        }

        void registerPhysicalPadPush(
                const int padIndex,
                const int bankIndex,
                const std::shared_ptr<lcdgui::ScreenComponent> screen,
                const input::ClientInput::Source inputSource,
                std::shared_ptr<sampler::Program> program)
        {
            assert(physicalPadPresses.count(padIndex) == 0);
            physicalPadPresses[padIndex] = { bankIndex, program, screen, inputSource };
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
        std::shared_ptr<mpc::input::KeyboardBindings> keyboardBindings;
        //
        // Maps physical pad index 0 - 15 to info at the time the pad was pushed.
        std::unordered_map<int, PhysicalPadPress> physicalPadPresses;
};

} // namespace mpc::controller
