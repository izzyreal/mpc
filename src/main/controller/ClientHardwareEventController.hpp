#pragma once

#include "hardware/ComponentId.hpp"

#include "client/event/ClientHardwareEvent.hpp"

#include "controller/ButtonLockTracker.hpp"

#include <cassert>

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

        void handleClientHardwareEvent(
            const mpc::client::event::ClientHardwareEvent &event);

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

        bool isNoteRepeatLockedOrPressed();

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
        void
        handleButtonDoublePress(const client::event::ClientHardwareEvent &);
    };

} // namespace mpc::controller
