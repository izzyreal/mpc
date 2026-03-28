#include "DirectorySaveTarget.hpp"

using namespace mpc;

DirectorySaveTarget::DirectorySaveTarget(mpc_fs::path basePath)
    : basePath(std::move(basePath))
{
}

mpc_fs::result<void>
DirectorySaveTarget::setFileData(const mpc_fs::path &path,
                                 const std::vector<char> &data)
{
    const auto absPath = basePath / path;
    if (data.empty())
    {
        const auto removeRes = mpc_fs::remove(absPath);
        if (!removeRes)
        {
            return tl::unexpected(removeRes.error());
        }
        return {};
    }

    return set_file_data(absPath, data);
}

mpc_fs::result<std::vector<char>>
DirectorySaveTarget::getFileData(const mpc_fs::path &path) const
{
    return get_file_data(basePath / path);
}

mpc_fs::result<bool> DirectorySaveTarget::exists(const mpc_fs::path &path) const
{
    return mpc_fs::exists(basePath / path);
}

mpc_fs::result<std::uintmax_t>
DirectorySaveTarget::fileSize(const mpc_fs::path &path) const
{
    return mpc_fs::file_size(basePath / path);
}
