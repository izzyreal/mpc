#include "VolumesPersistence.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"

#include <nlohmann/json.hpp>

using namespace mpc::nvram;
using namespace mpc::disk;
using json = nlohmann::json;

fs::path getVolumesPersistencePath(mpc::Mpc &mpc)
{
    return mpc.paths->configPath() / "volumes.json";
}

const size_t bufSize = 2048;

json read(mpc::Mpc &mpc)
{
    json result;

    const auto path = getVolumesPersistencePath(mpc);

    if (fs::exists(path))
    {
        const auto bytes = get_file_data(path);
        try
        {
            result = json::parse(bytes.begin(), bytes.end());
        }
        catch (...)
        {
            result = json::object();
        }
    }

    if (!result.is_object())
    {
        result = json::object();
    }

    if (!result.contains("volumes"))
    {
        result["volumes"] = json::array();
    }

    return result;
}

std::string VolumesPersistence::getPersistedActiveUUID(mpc::Mpc &mpc)
{
    json doc = read(mpc);
    auto &volumes = doc["volumes"];

    for (auto &vol : volumes)
    {
        auto uuid = vol["uuid"].get<std::string>();
        auto isActive = vol["active"].get<bool>();

        if (isActive)
        {
            return uuid;
        }
    }

    return "";
}

std::map<std::string, MountMode>
VolumesPersistence::getPersistedConfigs(mpc::Mpc &mpc)
{
    std::map<std::string, MountMode> persistedConfigs;

    json doc = read(mpc);
    auto &volumes = doc["volumes"];

    for (auto &vol : volumes)
    {
        auto uuid = vol["uuid"].get<std::string>();
        persistedConfigs[uuid] = static_cast<MountMode>(vol["mode"].get<int>());
    }

    return persistedConfigs;
}

void VolumesPersistence::save(mpc::Mpc &mpc)
{
    json d = read(mpc);
    auto &volumes = d["volumes"];

    std::vector<std::string> alreadyPersistedUUIDs;

    // Mark all inactive and collect existing UUIDs
    for (auto &vol : volumes)
    {
        vol["active"] = false;
        alreadyPersistedUUIDs.emplace_back(vol["uuid"].get<std::string>());
    }

    auto activeUUID = mpc.getDisk()->getVolume().volumeUUID;
    auto disks = mpc.getDisks();

    for (const auto &disk : disks)
    {
        const auto &diskVol = disk->getVolume();
        bool alreadyPersisted =
            std::find(begin(alreadyPersistedUUIDs), end(alreadyPersistedUUIDs),
                      diskVol.volumeUUID) != end(alreadyPersistedUUIDs);

        if (alreadyPersisted)
        {
            for (auto &vol : volumes)
            {
                if (vol["uuid"].get<std::string>() == diskVol.volumeUUID)
                {
                    vol["mode"] = diskVol.mode;
                    vol["active"] = (diskVol.volumeUUID == activeUUID);
                    break;
                }
            }
        }
        else
        {
            json volume = {{"uuid", diskVol.volumeUUID},
                           {"mode", diskVol.mode},
                           {"active", diskVol.volumeUUID == activeUUID}};
            volumes.push_back(volume);
        }
    }

    const auto data = d.dump(4); // pretty print (optional)
    const auto path = getVolumesPersistencePath(mpc);

    set_file_data(path, std::vector<char>(data.begin(), data.end()));
}
