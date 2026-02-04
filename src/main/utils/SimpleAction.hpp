#pragma once

#include "utils/SmallFn.hpp"

namespace mpc::utils
{
    // "Simple" means "no parameters, no return type"

    using SimpleAction = SmallFn<120, void()>;
} // namespace mpc::utils
