#pragma once

#include <map>
#include <string>

#include <disk/Volume.hpp>

namespace mpc
{
    class Mpc;
}

namespace mpc::nvram
{
    class VolumesPersistence
    {
    public:
        static void save(Mpc &);
        static std::map<std::string, disk::MountMode>
        getPersistedConfigs(Mpc &);
        static std::string getPersistedActiveUUID(Mpc &);
    };
} // namespace mpc::nvram
