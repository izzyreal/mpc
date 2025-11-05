#pragma once

#include "ConstrainedInt.hpp"

#include <cstdint>

namespace mpc
{

    using PhysicalPadIndex = ConstrainedInt<uint8_t, 0, 15>;
    using Velocity = ConstrainedInt<uint8_t, 0, 127>;
    using ProgramPadIndex = ConstrainedInt<uint8_t, 0, 63>;
    using MidiChannel = ConstrainedInt<uint8_t, 0, 15>;
    using Pressure = ConstrainedInt<uint8_t, 0, 127>;
    using NoteNumber = ConstrainedInt<uint8_t, 0, 127>;
    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;
    using VelocityOrPressure = ConstrainedInt<uint8_t, 0, 127>;
} // namespace mpc
