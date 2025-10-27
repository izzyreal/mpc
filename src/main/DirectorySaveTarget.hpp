#pragma once

#include "SaveTarget.hpp"
#include "mpc_fs.hpp"

#include <vector>

namespace mpc
{

    class DirectorySaveTarget : public SaveTarget
    {
        fs::path basePath;

    public:
        explicit DirectorySaveTarget(fs::path basePath);

        void setFileData(const fs::path &path, const std::vector<char> &data) override;

        std::vector<char> getFileData(const fs::path &path) const override;

        bool exists(const fs::path &path) const override;

        std::uintmax_t fileSize(const fs::path &path) const override;

        const fs::path &getBasePath() const
        {
            return basePath;
        }
    };

} // namespace mpc
