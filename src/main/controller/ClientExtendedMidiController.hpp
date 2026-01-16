#pragma once

#include "input/midi/MidiControlPresetV3.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "hardware/ComponentId.hpp"

#include <memory>
#include <map>

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::input::midi
{
    struct MidiControlPresetV3;
}

namespace mpc::controller
{
    class ClientHardwareEventController;

    class ClientExtendedMidiController
    {
    public:
        ClientExtendedMidiController(
            const std::shared_ptr<ClientHardwareEventController> &,
            const std::weak_ptr<sequencer::Sequencer> &);

        void handleEvent(const client::event::ClientMidiEvent &);

        std::shared_ptr<input::midi::MidiControlPresetV3> getActivePreset();
        void setActivePreset(
            const std::shared_ptr<input::midi::MidiControlPresetV3> &);

    private:
        std::shared_ptr<input::midi::MidiControlPresetV3> activePreset;
        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;
        std::weak_ptr<sequencer::Sequencer> sequencer;

        std::map<MidiNumber, MidiValue> statefulDataWheels;

        void pressButton(hardware::ComponentId id) const;

        void releaseButton(hardware::ComponentId id) const;

        void pressPad(hardware::ComponentId, float normalizedVelocity,
                      bool isAftertouch) const;

        void releasePad(hardware::ComponentId) const;

        void turnWheel(int) const;

        void moveSlider(float normalizedY) const;

        void movePot(hardware::ComponentId, float normalizedValue) const;
    };
} // namespace mpc::controller