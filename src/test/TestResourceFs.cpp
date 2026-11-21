#include "TestResourceFs.hpp"

#include <map>
#include <stdexcept>

#ifdef __APPLE__
#include "MacBundleResources.hpp"
#include "mpc_fs.hpp"
#else
#include <ResEmbed/ResEmbed.h>
#endif

namespace mpc_test_resources
{
namespace
{
std::string fixture_path(const std::string& path)
{
    constexpr auto prefix = "test/";
    if (path.compare(0, 5, prefix) != 0)
    {
        throw std::invalid_argument("Invalid test resource path: " + path);
    }
    return path.substr(5);
}
} // namespace

File FileSystem::open(const std::string& path) const
{
    const auto relative_path = fixture_path(path);
#ifdef __APPLE__
    const auto bundle_path = mpc::MacBundleResources::getResourcePath(path);
    const auto bytes = get_file_data(bundle_path);
    if (!bytes)
    {
        throw std::runtime_error("Test resource missing: " + path);
    }
    return File(std::string(bytes->begin(), bytes->end()));
#else
    const auto resources = ResEmbed::getCategory("mpctest");
    const auto it = resources.find(relative_path);
    if (it == resources.end())
    {
        throw std::runtime_error("Test resource missing: " + path);
    }
    const auto& data = it->second;
    if (data.empty())
    {
        return File(std::string{});
    }
    return File(std::string(data.asCharPointer(), data.size()));
#endif
}

std::vector<DirectoryEntry> FileSystem::iterate_directory(const std::string& path) const
{
    const auto relative_path = fixture_path(path + "/");
    std::map<std::string, bool> entries;
#ifdef __APPLE__
    const auto bundle_path = mpc::MacBundleResources::getResourcePath(path);
    const auto directory = mpc_fs::make_directory_iterator(bundle_path);
    if (!directory)
    {
        throw std::runtime_error("Test resource directory missing: " + path);
    }
    for (auto it = *directory; it != mpc_fs::directory_end(); ++it)
    {
        entries.emplace(it->path().filename().string(), it->is_directory());
    }
#else
    const auto resources = ResEmbed::getCategory("mpctest");
    for (const auto& [resource_path, data] : resources)
    {
        if (resource_path.compare(0, relative_path.size(), relative_path) != 0)
        {
            continue;
        }
        const auto remaining = resource_path.substr(relative_path.size());
        const auto separator = remaining.find('/');
        const auto name = remaining.substr(0, separator);
        if (!name.empty())
        {
            entries[name] = separator != std::string::npos;
        }
    }
    if (entries.empty())
    {
        throw std::runtime_error("Test resource directory missing: " + path);
    }
#endif
    std::vector<DirectoryEntry> result;
    result.reserve(entries.size());
    for (const auto& [name, directory] : entries)
    {
        result.emplace_back(name, directory);
    }
    return result;
}

FileSystem get_filesystem()
{
    return {};
}
} // namespace mpc_test_resources
