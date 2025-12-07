#pragma once

#include "utils/SmallFn.hpp"

namespace mpc::utils
{
    // "Simple" means "no parameters, no return type"

    using SimpleAction = SmallFn<96, void()>;
    using SmallSimpleAction = SmallFn<32, void()>;
}

