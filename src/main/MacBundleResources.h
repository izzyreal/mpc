#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#if not TARGET_OS_IPHONE

#include <string>

namespace mpc {
    class MacBundleResources {
    public:
        static std::string getResourcePath(const std::string &resourceName);
    };
}

#endif
#endif
