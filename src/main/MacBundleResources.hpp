#pragma once

#ifdef __APPLE__

#include <string>

namespace mpc {
    class MacBundleResources {
    public:
        static std::string getResourcePath(const std::string &resourceName);
    };
}

#endif
