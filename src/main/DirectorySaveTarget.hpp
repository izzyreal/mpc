#pragma once

#include "SaveTarget.hpp"
#include "mpc_fs.hpp"

#include <vector>

namespace mpc
{

    class DirectorySaveTarget : public SaveTarget
    {
        mpc_fs::path basePath;

    public:
        explicit DirectorySaveTarget(mpc_fs::path basePath);

        void setFileData(const mpc_fs::path &path,
                         const std::vector<char> &data) override;

        std::vector<char> getFileData(const mpc_fs::path &path) const override;

        bool exists(const mpc_fs::path &path) const override;

        std::uintmax_t fileSize(const mpc_fs::path &path) const override;

        const mpc_fs::path &getBasePath() const
        {
            return basePath;
        }
    };

} // namespace mpc
