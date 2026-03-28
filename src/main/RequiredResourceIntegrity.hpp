#pragma once

#include "MpcResourceUtil.hpp"

#include <vector>

namespace mpc
{
    class RequiredResourceIntegrity
    {
    public:
        static std::vector<MissingRequiredResource> check();
    };
} // namespace mpc
