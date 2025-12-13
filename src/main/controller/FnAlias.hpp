#pragma once

#include "utils/SmallFn.hpp"
#include "IntTypes.hpp"

namespace mpc::controller
{
    using SetSelectedNoteFn = utils::SmallFn<8, void(DrumNoteNumber)>;
    using SetSelectedPadFn = utils::SmallFn<8, void(ProgramPadIndex)>;
} // namespace mpc::controller
