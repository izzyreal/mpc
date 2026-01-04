#pragma once

#include "input/midi/MidiControlPresetV3.hpp"

#include <memory>

namespace mpc::controller
{
    class ClientExtendedMidiController
    {
    public:
        ClientExtendedMidiController();

    private:
        std::shared_ptr<input::midi::MidiControlPresetV3> activePreset;
    };
} // namespace mpc::controller