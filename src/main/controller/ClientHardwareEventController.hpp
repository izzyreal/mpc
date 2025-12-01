#pragma once

#include "IntTypes.hpp"
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

        bool isRecLocked() const
        {
            return buttonLockTracker.isLocked(hardware::ComponentId::REC);
        }

        bool isOverdubLocked() const
        {
            return buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB);
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

        bool isRecLockedOrPressed() const;

        bool isOverdubLockedOrPressed() const;

        TimeInMilliseconds getMostRecentPhysicalPadPressTime() const;
        Velocity getMostRecentPhysicalPadPressVelocity() const;

    protected:
        // Maps component label to accumulated delta for discretization
        std::unordered_map<hardware::ComponentId, float> deltaAccumulators;

    private:
        Mpc &mpc;

        // The velocity of the most recent physical pad press. This is used
        // in the Velocity Modulation, Velo/Env >> Filter, and Velo >> Pitch
        // windows.
        std::atomic<Velocity> mostRecentPhysicalPadPressVelocity{MaxVelocity};
        std::atomic<TimeInMilliseconds> mostRecentPhysicalPadPressTimeMs{0};

        void updateMostRecentPhysicalPadPressVelocity(Velocity);

        void handlePadPress(const client::event::ClientHardwareEvent &);
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
