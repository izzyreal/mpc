#include "ResourceUtil.h"

#include "Logger.hpp"

#ifdef __APPLE__
#include <TargetConditionals.h>
#if not TARGET_OS_IPHONE
#define MAC_BUNDLE_RESOURCES 1
#endif
#endif

#ifdef MAC_BUNDLE_RESOURCES

#include "mpc_fs.hpp"

#include "MacBundleResources.hpp"

std::vector<char> get_resource_data_from_mac_os_bundle(const std::string &path)
{
    auto resource_path = getMacBundleResourcesPath() + "/" + path;

    return get_file_data(resource_path);
}
#else

#include <cmrc/cmrc.hpp>
#include <string_view>
CMRC_DECLARE(mpc);

std::vector<char> get_resource_data_from_in_memory_filesystem(const std::string& path)
{
    const auto file = cmrc::mpc::get_filesystem().open(path.c_str());
    const auto data = std::string_view(file.begin(), file.size()).data();
    const std::vector<char> data_vec(data, data + file.size());
    return data_vec;
}
#endif

using namespace mpc;

std::vector<char> ResourceUtil::get_resource_data(const std::string& path)
{
    try {
#ifdef MAC_BUNDLE_RESOURCES
        return get_resource_data_from_mac_os_bundle(path);
#else
        return get_resource_data_from_in_memory_filesystem(path);
#endif
    }
    catch (const std::exception& e)
    {
        MLOG("Exception while reading resource: " + path);
        const auto message = e.what();
        MLOG(message);
    }
    return {};
}
