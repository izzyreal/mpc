#pragma once

#include "utils/SmallFn.hpp"

namespace mpc::utils
{
    // "Simple" means "no parameters, no return type"

    using SimpleAction = SmallFn<104, void()>;
} // namespace mpc::utils
