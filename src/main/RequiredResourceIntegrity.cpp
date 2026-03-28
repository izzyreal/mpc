#include "RequiredResourceIntegrity.hpp"

#include "MpcRequiredResources.hpp"

using namespace mpc;

std::vector<MissingRequiredResource> RequiredResourceIntegrity::check()
{
    std::vector<MissingRequiredResource> missing;

    for (const auto path : required_resources::paths)
    {
        const std::string logicalPath(path);
        if (MpcResourceUtil::resource_exists(logicalPath))
        {
            continue;
        }

        missing.push_back(
            MissingRequiredResource{logicalPath,
                                    MpcResourceUtil::resolve_resource_path(
                                        logicalPath)});
    }

    return missing;
}
