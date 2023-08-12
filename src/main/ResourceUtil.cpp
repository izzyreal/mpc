#include "ResourceUtil.h"

#include "Logger.hpp"

#ifdef MAC_BUNDLE_RESOURCES
#include "MacBundleResources.h"
#include "mpc_fs.hpp"
#else
#include <cmrc/cmrc.hpp>
#include <string_view>
CMRC_DECLARE(mpc);
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

#ifdef MAC_BUNDLE_RESOURCES
std::vector<char> ResourceUtil::get_resource_data_from_mac_os_bundle(const std::string &path)
{
    const auto resource_path = mpc::MacBundleResources::getResourcePath(path);
    return get_file_data(resource_path);
}
#else
std::vector<char> ResourceUtil::get_resource_data_from_in_memory_filesystem(const std::string& path)
{
  const auto file = cmrc::mpc::get_filesystem().open(path.c_str());
  const auto data = std::string_view(file.begin(), file.size()).data();
  const std::vector<char> data_vec(data, data + file.size());
  return data_vec;
}
#endif