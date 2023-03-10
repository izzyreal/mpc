#pragma once

#include <string>

namespace mpc {

    class Mpc;

    class AutoSave {
    public:
        static void restoreAutoSavedState(mpc::Mpc&, const std::string& overridePath = "");
        static void storeAutoSavedState(mpc::Mpc&, const std::string& overridePath = "");

    };
}