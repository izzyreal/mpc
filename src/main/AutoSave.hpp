#pragma once

#include "SaveTarget.hpp"

#include <memory>

namespace mpc
{
    class Mpc;

    class AutoSave
    {
    public:
        static void restoreAutoSavedState(Mpc &, std::shared_ptr<SaveTarget>,
                                          bool headless);

        static void storeAutoSavedState(Mpc &, std::shared_ptr<SaveTarget>);
    };
} // namespace mpc
