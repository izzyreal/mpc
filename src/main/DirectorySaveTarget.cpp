#include "DirectorySaveTarget.hpp"

using namespace mpc;

DirectorySaveTarget::DirectorySaveTarget(mpc_fs::path basePath)
    : basePath(std::move(basePath))
{
}

void DirectorySaveTarget::setFileData(const mpc_fs::path &path,
                                      const std::vector<char> &data)
{
    const auto absPath = basePath / path;
    if (data.empty())
    {
        mpc_fs::remove(absPath);
    }
    else
    {
        set_file_data(absPath, data);
    }
}

std::vector<char> DirectorySaveTarget::getFileData(const mpc_fs::path &path) const
{
    return get_file_data(basePath / path);
}

bool DirectorySaveTarget::exists(const mpc_fs::path &path) const
{
    return mpc_fs::exists(basePath / path);
}

std::uintmax_t DirectorySaveTarget::fileSize(const mpc_fs::path &path) const
{
    return mpc_fs::file_size(basePath / path);
}
