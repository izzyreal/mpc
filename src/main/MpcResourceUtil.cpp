#include "MpcResourceUtil.hpp"

#include "Logger.hpp"

#ifdef __APPLE__
#include "MacBundleResources.hpp"
#define MAC_BUNDLE_RESOURCES 1
#endif

#ifdef MAC_BUNDLE_RESOURCES

#include "mpc_fs.hpp"

std::string resolve_resource_path_from_mac_os_bundle(const std::string &path)
{
    return mpc::MacBundleResources::getResourcePath(path);
}

std::vector<char> get_resource_data_from_mac_os_bundle(const std::string &path)
{
    const auto resource_path = resolve_resource_path_from_mac_os_bundle(path);

    return get_file_data(resource_path).value_or(std::vector<char>{});
}

bool resource_exists_in_mac_os_bundle(const std::string &path)
{
    const auto resource_path = resolve_resource_path_from_mac_os_bundle(path);
    return mpc_fs::exists(resource_path).value_or(false);
}
#else

#include <cmrc/cmrc.hpp>
#include <string_view>
CMRC_DECLARE(mpc);

std::vector<char>
get_resource_data_from_in_memory_filesystem(const std::string &path)
{
    const auto file = cmrc::mpc::get_filesystem().open(path.c_str());
    const auto data = std::string_view(file.begin(), file.size()).data();
    const std::vector<char> data_vec(data, data + file.size());
    return data_vec;
}

bool resource_exists_in_memory_filesystem(const std::string &path)
{
    try
    {
        (void) cmrc::mpc::get_filesystem().open(path.c_str());
        return true;
    }
    catch (...)
    {
        return false;
    }
}
#endif

using namespace mpc;

std::string MpcResourceUtil::resolve_resource_path(const std::string &path)
{
#ifdef MAC_BUNDLE_RESOURCES
    return resolve_resource_path_from_mac_os_bundle(path);
#else
    return path;
#endif
}

std::vector<char> MpcResourceUtil::get_resource_data(const std::string &path)
{
    try
    {
#ifdef MAC_BUNDLE_RESOURCES
        return get_resource_data_from_mac_os_bundle(path);
#else
        return get_resource_data_from_in_memory_filesystem(path);
#endif
    }
    catch (const std::exception &e)
    {
        MLOG("Exception while reading resource: " + path);
        const auto message = e.what();
        MLOG(message);
    }
    return {};
}

bool MpcResourceUtil::resource_exists(const std::string &path)
{
#ifdef MAC_BUNDLE_RESOURCES
    return resource_exists_in_mac_os_bundle(path);
#else
    return resource_exists_in_memory_filesystem(path);
#endif
}
