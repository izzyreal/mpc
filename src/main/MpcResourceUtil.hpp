#pragma once

#include <vector>
#include <string>

namespace mpc
{
    struct MissingRequiredResource
    {
        std::string logicalPath;
        std::string resolvedPath;
    };

    class MpcResourceUtil
    {

    public:
        static std::vector<char> get_resource_data(const std::string &path);
        static bool resource_exists(const std::string &path);
        static std::string resolve_resource_path(const std::string &path);
    };
} // namespace mpc
