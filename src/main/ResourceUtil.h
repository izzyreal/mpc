#pragma once

#include <vector>
#include <string>

#ifdef __APPLE__

#include <TargetConditionals.h>

#if not TARGET_OS_IPHONE

#include "MacBundleResources.h"

#define MAC_BUNDLE_RESOURCES 1
#endif
#endif

namespace mpc {
    class ResourceUtil {

    public:
        static std::vector<char> get_resource_data(const std::string &path);

    private:
#ifdef MAC_BUNDLE_RESOURCES

        static std::vector<char> get_resource_data_from_mac_os_bundle(const std::string &path);

#else
        static std::vector<char> get_resource_data_from_in_memory_filesystem(const std::string& path);
#endif
    };
}
