#include "DirectorySaveTarget.hpp"

using namespace mpc;

DirectorySaveTarget::DirectorySaveTarget(fs::path basePath)
    : basePath(std::move(basePath)) {}

void DirectorySaveTarget::setFileData(const fs::path &path, const std::vector<char> &data)
{
    const auto absPath = basePath / path;
    if (data.empty())
    {
        fs::remove(absPath);
    }
    else
    {
        set_file_data(absPath, data);
    }
}

std::vector<char> DirectorySaveTarget::getFileData(const fs::path &path) const
{
    return get_file_data(basePath / path);
}

bool DirectorySaveTarget::exists(const fs::path &path) const
{
    return fs::exists(basePath / path);
}

std::uintmax_t DirectorySaveTarget::fileSize(const fs::path &path) const
{
    return fs::file_size(basePath / path);
}
