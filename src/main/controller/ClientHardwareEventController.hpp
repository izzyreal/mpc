#pragma once

#include "hardware/ComponentId.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "client/event/PhysicalPadPress.hpp"

#include "controller/ButtonLockTracker.hpp"

#include "lcdgui/ScreenComponent.hpp"

#include <cassert>
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::controller
{

    class ClientHardwareEventController
    {
    public:
        ClientHardwareEventController(mpc::Mpc &);

        void handleClientHardwareEvent(const mpc::client::event::ClientHardwareEvent &event);

        ButtonLockTracker buttonLockTracker;

        bool isNoteRepeatLocked() const
        {
            return buttonLockTracker.isLocked(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
        }
        void lockNoteRepeat()
        {
            buttonLockTracker.lock(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
        }
        void unlockNoteRepeat()
        {
            buttonLockTracker.unlock(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
        }

        bool isPhysicallyPressed(const int physicalPadIndex, const Bank bank) const
        {
            return physicalPadPresses.count(physicalPadIndex) > 0 &&
                   physicalPadPresses.at(physicalPadIndex).bank == bank;
        }

        bool isPhysicallyPressedDueToKeyboardInput(const int physicalPadIndex) const
        {
            if (physicalPadPresses.count(physicalPadIndex) == 0)
            {
                return false;
            }

            const auto pressSource = physicalPadPresses.at(physicalPadIndex).inputSource;

            return pressSource == client::event::ClientHardwareEvent::Source::HostInputKeyboard;
        }

        void registerPhysicalPadPush(
            const int padIndex,
            const Bank bank,
            const std::shared_ptr<lcdgui::ScreenComponent> screen,
            const client::event::ClientHardwareEvent::Source inputSource,
            const std::optional<int> drumIndex)
        {
            assert(physicalPadPresses.count(padIndex) == 0);
            physicalPadPresses[padIndex] = {bank, screen, inputSource, drumIndex};
        }

        client::event::PhysicalPadPress registerPhysicalPadRelease(const int padIndex)
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
        mpc::Mpc &mpc;

        void handlePadPress(const client::event::ClientHardwareEvent &);
        void handlePadAftertouch(const client::event::ClientHardwareEvent &);
        void handlePadRelease(const client::event::ClientHardwareEvent &);
        void handleDataWheel(const client::event::ClientHardwareEvent &);
        void handleSlider(const client::event::ClientHardwareEvent &);
        void handlePot(const client::event::ClientHardwareEvent &);
        void handleButtonPress(const client::event::ClientHardwareEvent &);
        void handleButtonRelease(const client::event::ClientHardwareEvent &);
        void handleButtonDoublePress(const client::event::ClientHardwareEvent &);

        // Maps physical pad index 0 - 15 to info at the time the pad was pushed.
        std::unordered_map<int, client::event::PhysicalPadPress> physicalPadPresses;

    };

} // namespace mpc::controller
