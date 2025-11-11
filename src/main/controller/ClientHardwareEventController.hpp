#pragma once

#include "hardware/ComponentId.hpp"

#include "client/event/ClientHardwareEvent.hpp"

#include "controller/ButtonLockTracker.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::controller
{

    class ClientHardwareEventController
    {
    public:
        explicit ClientHardwareEventController(Mpc &);

        void handleClientHardwareEvent(
            const client::event::ClientHardwareEvent &event);

        ButtonLockTracker buttonLockTracker;

        bool isNoteRepeatLocked() const
        {
            return buttonLockTracker.isLocked(
                hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
        }
        void lockNoteRepeat()
        {
            buttonLockTracker.lock(
                hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
        }
        void unlockNoteRepeat()
        {
            buttonLockTracker.unlock(
                hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
        }

        bool isNoteRepeatLockedOrPressed() const;

    protected:
        // Maps component label to accumulated delta for discretization
        std::unordered_map<hardware::ComponentId, float> deltaAccumulators;

    private:
        Mpc &mpc;

        void handlePadPress(const client::event::ClientHardwareEvent &) const;
        void
        handlePadAftertouch(const client::event::ClientHardwareEvent &) const;
        void handlePadRelease(const client::event::ClientHardwareEvent &) const;
        void handleDataWheel(const client::event::ClientHardwareEvent &);
        void handleSlider(const client::event::ClientHardwareEvent &) const;
        void handlePot(const client::event::ClientHardwareEvent &) const;
        void
        handleButtonPress(const client::event::ClientHardwareEvent &) const;
        void
        handleButtonRelease(const client::event::ClientHardwareEvent &) const;
        void
        handleButtonDoublePress(const client::event::ClientHardwareEvent &);
    };

} // namespace mpc::controller
