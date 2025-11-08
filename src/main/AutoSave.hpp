#pragma once

#include "SaveTarget.hpp"

#include <memory>

namespace mpc
{
    class Mpc;

    class AutoSave
    {
    public:
        static void restoreAutoSavedState(
            mpc::Mpc &, std::shared_ptr<mpc::SaveTarget>, const bool headless);

        static void
        storeAutoSavedState(mpc::Mpc &,
                                      const std::shared_ptr<mpc::SaveTarget>);
    };
} // namespace mpc
