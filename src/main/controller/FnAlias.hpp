#pragma once

#include "utils/SmallFn.hpp"
#include "IntTypes.hpp"
#include "controller/Bank.hpp"

namespace mpc::controller
{
    using SetSelectedNoteFn = utils::SmallFn<8, void(DrumNoteNumber)>;
    using SetSelectedPadFn = utils::SmallFn<8, void(ProgramPadIndex)>;
    using SetActiveBankUiCallback = utils::SmallFn<8, void(Bank)>;

} // namespace mpc::controller
