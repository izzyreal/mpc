#pragma once

namespace mpc
{

    class Mpc;

    class AutoSave
    {
    public:
        static void restoreAutoSavedState(mpc::Mpc &);
        static void storeAutoSavedState(mpc::Mpc &);
    };
} // namespace mpc
