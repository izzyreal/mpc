#pragma once

#include "mpc_fs.hpp"

#include <vector>

namespace mpc
{
    class SaveTarget
    {
    public:
        virtual ~SaveTarget() = default;

        virtual mpc_fs::result<void>
        setFileData(const mpc_fs::path &path,
                    const std::vector<char> &data) = 0;
        virtual mpc_fs::result<std::vector<char>>
        getFileData(const mpc_fs::path &path) const = 0;
        virtual mpc_fs::result<bool> exists(const mpc_fs::path &path) const = 0;
        virtual mpc_fs::result<std::uintmax_t>
        fileSize(const mpc_fs::path &path) const = 0;
    };

} // namespace mpc
