#include "VolumesPersistence.hpp"

#include "Mpc.hpp"
#include <disk/AbstractDisk.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

using namespace mpc::nvram;
using namespace mpc::disk;
using namespace rapidjson;

fs::path getVolumesPersistencePath(mpc::Mpc& mpc) { return mpc.paths->configPath() / "volumes.json"; }

const size_t bufSize = 2048;

Document read(mpc::Mpc& mpc)
{
    Document result;

    const auto path = getVolumesPersistencePath(mpc);
    
    if (fs::exists(path))
    {
        const auto bytes = get_file_data(path);
        result.Parse(bytes.data(), bytes.size());
    }
    
    if (!result.IsObject())
    {
        result.SetObject();
    }
    
    if (!result.HasMember("volumes"))
    {
        result.AddMember("volumes", Value().SetArray(), result.GetAllocator());
    }

    return result;
}

std::string VolumesPersistence::getPersistedActiveUUID(mpc::Mpc& mpc)
{
    Document doc = read(mpc);
    Value& volumes = doc["volumes"];

    for (auto i = volumes.Begin(); i != volumes.End(); i++)
    {
        auto uuid = (*i)["uuid"].GetString();
        auto isActive = (*i)["active"].GetBool();
        
        if (isActive)
            return uuid;
    }

    return "";
}

std::map<std::string, MountMode> VolumesPersistence::getPersistedConfigs(mpc::Mpc& mpc)
{
    std::map<std::string, MountMode> persistedConfigs;
    
    Document doc = read(mpc);
    Value& volumes = doc["volumes"];
    
    for (auto i = volumes.Begin(); i != volumes.End(); i++)
    {
        auto uuid = (*i)["uuid"].GetString();
        persistedConfigs[std::string(uuid)] = (MountMode)(*i)["mode"].GetInt();
    }
    
    return persistedConfigs;
}

void VolumesPersistence::save(mpc::Mpc & mpc)
{
    Document d = read(mpc);
        
    Value& volumes = d["volumes"];
    
    std::vector<std::string> alreadyPersistedUUIDs;
    
    for (auto i = volumes.Begin(); i != volumes.End(); i++)
    {
        (*i)["active"].Swap(Value().SetBool(false));
        auto uuid = (*i)["uuid"].GetString();
        alreadyPersistedUUIDs.emplace_back(uuid);
    }
    
    auto activeUUID = mpc.getDisk()->getVolume().volumeUUID;
    
    auto disks = mpc.getDisks();
        
    for (const auto & disk : disks)
    {
        auto& diskVol = disk->getVolume();
     
        bool alreadyPersisted = std::find(begin(alreadyPersistedUUIDs), end(alreadyPersistedUUIDs), diskVol.volumeUUID) != end(alreadyPersistedUUIDs);
        
        if (alreadyPersisted)
        {
            for (auto j = volumes.GetArray().Begin(); j != volumes.GetArray().End(); j++)
            {
                auto uuid = (*j)["uuid"].GetString();
                if (std::string(uuid) == diskVol.volumeUUID)
                {
                    (*j)["mode"].Swap(Value().SetInt(diskVol.mode));
                    (*j)["active"].Swap(Value().SetBool(uuid == activeUUID));
                    break;
                }
            }
        }
        else
        {
            Value volume(kObjectType);
            volume.AddMember("uuid", StringRef(diskVol.volumeUUID.c_str()), d.GetAllocator());
            volume.AddMember("mode", diskVol.mode, d.GetAllocator());
            volume.AddMember("active", Value().SetBool(diskVol.volumeUUID == activeUUID), d.GetAllocator());
            volumes.PushBack(volume, d.GetAllocator());
        }
    }

    StringBuffer buffer;

    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    const char* data = buffer.GetString();

    const auto path = getVolumesPersistencePath(mpc);
    set_file_data(path, std::vector<char>{data, data + buffer.GetSize() });
}
