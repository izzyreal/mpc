#pragma once

#include "input/midi/MidiControlPresetV3.hpp"

namespace mpc::controller
{
    class ClientExtendedMidiController
    {
    public:
        ClientExtendedMidiController();

    private:
        std::unique_ptr<input::midi::MidiControlPresetV3> activePreset;
    };
}