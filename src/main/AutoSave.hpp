#pragma once

#include "SaveTarget.hpp"

#include <memory>

namespace mpc
{
    class Mpc;

    class AutoSave
    {
    public:
        static void
        restoreAutoSavedStateWithTarget(mpc::Mpc &,
                                        std::shared_ptr<mpc::SaveTarget>);
        static void
        storeAutoSavedStateWithTarget(mpc::Mpc &,
                                      std::shared_ptr<mpc::SaveTarget>);
    };
} // namespace mpc
