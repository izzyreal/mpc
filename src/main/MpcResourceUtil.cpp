#include "MpcResourceUtil.hpp"

#include "Logger.hpp"

#ifdef __APPLE__
#include "MacBundleResources.hpp"
#define MAC_BUNDLE_RESOURCES 1
#endif

#ifdef MAC_BUNDLE_RESOURCES

#include "mpc_fs.hpp"
#include <system_error>

std::vector<char> get_resource_data_from_mac_os_bundle(const std::string &path)
{
    const auto resource_path = mpc::MacBundleResources::getResourcePath(path);

    MLOG("MpcResourceUtil resource request: " + path);
    MLOG("MpcResourceUtil resolved path: " + resource_path);

    std::error_code ec;
    const bool exists = fs::exists(resource_path, ec);
    if (ec)
    {
        MLOG("MpcResourceUtil exists() error: " + ec.message());
        ec.clear();
    }

    if (!exists)
    {
        MLOG("MpcResourceUtil resource missing at resolved path");
        return {};
    }

    const auto size = fs::file_size(resource_path, ec);
    if (ec)
    {
        MLOG("MpcResourceUtil file_size() error: " + ec.message());
        ec.clear();
    }
    else
    {
        MLOG("MpcResourceUtil file size: " + std::to_string(static_cast<unsigned long long>(size)));
    }

    auto data = get_file_data(resource_path);
    MLOG("MpcResourceUtil loaded bytes: " + std::to_string(data.size()));
    return data;
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
#endif

using namespace mpc;

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
