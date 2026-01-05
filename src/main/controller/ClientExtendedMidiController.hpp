#pragma once

#include "input/midi/MidiControlPresetV3.hpp"

#include <memory>

namespace mpc::input::midi
{
    struct MidiControlPresetV3;
}

namespace mpc::controller
{
    class ClientExtendedMidiController
    {
    public:
        ClientExtendedMidiController();

    std::shared_ptr<input::midi::MidiControlPresetV3> getActivePreset();
    void setActivePreset(std::shared_ptr<input::midi::MidiControlPresetV3>);

    private:
        std::shared_ptr<input::midi::MidiControlPresetV3> activePreset;
    };
} // namespace mpc::controller