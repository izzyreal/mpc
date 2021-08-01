#include "VolumesPersistence.hpp"

#include <Mpc.hpp>
#include <Paths.hpp>
#include <disk/AbstractDisk.hpp>

#include <rapidjson/document.h>
#include "rapidjson/filewritestream.h"
#include <rapidjson/writer.h>

using namespace mpc::nvram;
using namespace mpc::disk;
using namespace rapidjson;

const auto volumesPersistencePath = mpc::Paths::configPath() + "volumes.json";
const size_t bufSize = 2048;

Document read()
{
    Document result;
    FILE* fp = fopen(volumesPersistencePath.c_str(), "r");
    
    if (fp != NULL)
    {
        char buf[bufSize];
        fgets(buf, bufSize, fp);
        result.Parse(buf);
        fclose(fp);
    }
    
    if (!result.IsObject())
        result.SetObject();
    
    if (!result.HasMember("volumes"))
        result.AddMember("volumes", Value().SetArray(), result.GetAllocator());
    
    return result;
}

std::map<std::string, MountMode> VolumesPersistence::getPersistedConfigs()
{
    std::map<std::string, MountMode> persistedConfigs;
    
    Value& volumes = read()["volumes"];
        
    for (auto i = volumes.Begin(); i != volumes.End(); i++)
    {
        auto uuid = (*i)["uuid"].GetString();
        persistedConfigs[std::string(uuid)] = (MountMode)(*i)["mode"].GetInt();
    }
    
    return persistedConfigs;
}

void VolumesPersistence::save(mpc::Mpc & mpc)
{
    Document d = read();
        
    Value& volumes = d["volumes"];
    
    std::vector<std::string> alreadyPersistedUUIDs;
    
    for (auto i = volumes.Begin(); i != volumes.End(); i++)
    {
        auto uuid = (*i)["uuid"].GetString();
        alreadyPersistedUUIDs.emplace_back(std::string(uuid));
    }
    
    auto disks = mpc.getDisks();
        
    for (int i = 0; i < disks.size(); i++)
    {
        auto& diskVol = disks[i]->getVolume();
     
        bool alreadyPersisted = std::find(begin(alreadyPersistedUUIDs), end(alreadyPersistedUUIDs), diskVol.volumeUUID) != end(alreadyPersistedUUIDs);
        
        if (alreadyPersisted)
        {
            for (auto i = volumes.GetArray().Begin(); i != volumes.GetArray().End(); i++)
            {
                auto uuid = (*i)["uuid"].GetString();
                if (std::string(uuid) == diskVol.volumeUUID)
                {
                    (*i)["mode"].Swap(Value().SetInt(diskVol.mode));
                    break;
                }
            }
        }
        else
        {
            Value volume(kObjectType);
            volume.AddMember("uuid", StringRef(diskVol.volumeUUID.c_str()), d.GetAllocator());
            volume.AddMember("mode", diskVol.mode, d.GetAllocator());
            volumes.PushBack(volume, d.GetAllocator());
        }
    }
    
    FILE* fp = fopen(volumesPersistencePath.c_str(), "w");
    
    char writeBuffer[bufSize];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    
    Writer<FileWriteStream> writer(os);
    
    d.Accept(writer);
    fclose(fp);
}
