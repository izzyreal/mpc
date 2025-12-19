#pragma once

#include "mpc_fs.hpp"

#include <vector>

namespace mpc
{
    class SaveTarget
    {
    public:
        virtual ~SaveTarget() = default;

        virtual void setFileData(const fs::path &path,
                                 const std::vector<char> &data) = 0;
        virtual std::vector<char> getFileData(const fs::path &path) const = 0;
        virtual bool exists(const fs::path &path) const = 0;
        virtual std::uintmax_t fileSize(const fs::path &path) const = 0;
    };

} // namespace mpc
