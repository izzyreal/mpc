#include "VolumesPersistence.hpp"

#include "FileIoPolicy.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <random>

using namespace mpc::nvram;
using namespace mpc::file_io;
using namespace mpc::disk;
using json = nlohmann::json;

mpc_fs::path getVolumesPersistencePath(mpc::Mpc &mpc)
{
    return mpc.paths->configPath() / "volumes.json";
}

const size_t bufSize = 2048;

json read(mpc::Mpc &mpc)
{
    json result;

    const auto path = getVolumesPersistencePath(mpc);
    const auto existsValue = value(
        mpc_fs::exists(path), FailurePolicy::Recoverable,
        "read volumes persistence existence check for '" + path.string() + "'");
    if (existsValue && *existsValue)
    {
        const auto bytesValue =
            value(get_file_data(path), FailurePolicy::Recoverable,
                  "read volumes persistence file for '" + path.string() + "'");
        if (!bytesValue)
        {
            result = json::object();
        }
        else
        {
            try
            {
                result = json::parse(bytesValue->begin(), bytesValue->end());
            }
            catch (...)
            {
                MLOG("VolumesPersistence::read found invalid JSON in '" +
                     path.string() + "'");
                result = json::object();
            }
        }
    }

    if (!result.is_object())
    {
        result = json::object();
    }

    if (!result.contains("volumes") || !result["volumes"].is_array())
    {
        result["volumes"] = json::array();
    }

    return result;
}

std::string VolumesPersistence::getPersistedActiveUUID(Mpc &mpc)
{
    json doc = read(mpc);
    auto &volumes = doc["volumes"];

    for (auto &vol : volumes)
    {
        if (!vol.is_object() || !vol.contains("uuid") ||
            !vol["uuid"].is_string() || !vol.contains("active") ||
            !vol["active"].is_boolean())
        {
            continue;
        }

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
VolumesPersistence::getPersistedConfigs(Mpc &mpc)
{
    std::map<std::string, MountMode> persistedConfigs;

    json doc = read(mpc);
    auto &volumes = doc["volumes"];

    for (auto &vol : volumes)
    {
        if (!vol.is_object() || !vol.contains("uuid") ||
            !vol["uuid"].is_string() || !vol.contains("mode") ||
            !vol["mode"].is_number_integer())
        {
            continue;
        }

        auto uuid = vol["uuid"].get<std::string>();
        if (vol["mode"] >= DISABLED && vol["mode"] <= READ_WRITE)
        {
            persistedConfigs[uuid] =
                static_cast<MountMode>(vol["mode"].get<int>());
        }
    }

    return persistedConfigs;
}

std::vector<Volume> VolumesPersistence::getPersistedImages(Mpc &mpc)
{
    std::vector<Volume> result;
    const auto document = read(mpc);
    for (const auto &v : document["volumes"])
    {
        if (!v.is_object() || !v.contains("type") || v["type"] != "image" ||
            !v.contains("uuid") || !v["uuid"].is_string() ||
            !v.contains("path") || !v["path"].is_string() ||
            !v.contains("label") || !v["label"].is_string() ||
            !v.contains("mode") || !v["mode"].is_number_integer())
        {
            continue;
        }
        Volume volume;
        volume.type = DISK_IMAGE;
        volume.volumeUUID = v["uuid"].get<std::string>();
        volume.diskImagePath = v["path"].get<std::string>();
        volume.label = v["label"].get<std::string>();
        if (volume.volumeUUID.empty() ||
            volume.volumeUUID == "default_volume" ||
            volume.diskImagePath.empty())
        {
            continue;
        }
        if (v["mode"] < DISABLED || v["mode"] > READ_WRITE)
        {
            continue;
        }
        volume.mode = static_cast<MountMode>(v["mode"].get<int>());
        if (v.contains("access") && v["access"].is_string())
        {
            volume.diskImageAccessToken = v["access"].get<std::string>();
        }
        if (v.contains("size") && v["size"].is_number_unsigned())
        {
            volume.volumeSize = v["size"].get<uint64_t>();
        }
        const auto duplicate =
            std::any_of(result.begin(), result.end(),
                        [&](const auto &other)
                        {
                            return other.volumeUUID == volume.volumeUUID ||
                                   other.diskImagePath == volume.diskImagePath;
                        });
        if (!duplicate)
        {
            result.push_back(std::move(volume));
        }
    }
    return result;
}

bool VolumesPersistence::save(Mpc &mpc)
{
    json document = read(mpc);
    json volumes = json::array();
    // Retain disconnected USB settings. Image bindings are explicitly managed
    // and removed records must not be resurrected on restart.
    const auto disks = mpc.getDisks();
    for (auto volume : document["volumes"])
    {
        if (!volume.is_object() || !volume.contains("uuid") ||
            !volume["uuid"].is_string())
        {
            continue;
        }
        if (volume.contains("type") && volume["type"] == "image")
        {
            continue;
        }
        const auto uuid = volume["uuid"].get<std::string>();
        if (std::any_of(disks.begin(), disks.end(),
                        [&](const auto &d)
                        {
                            return d->getVolume().volumeUUID == uuid;
                        }))
        {
            continue;
        }
        volume["active"] = false;
        volumes.push_back(volume);
    }
    const auto active = mpc.getDisk();
    for (const auto &disk : disks)
    {
        const auto &v = disk->getVolume();
        json record = {{"uuid", v.volumeUUID},
                       {"mode", v.mode},
                       {"active", disk == active}};
        if (v.type == DISK_IMAGE)
        {
            record["type"] = "image";
            record["path"] = v.diskImagePath;
            record["access"] = v.diskImageAccessToken;
            record["label"] = v.label;
            record["size"] = v.volumeSize;
        }
        volumes.push_back(std::move(record));
    }
    document["volumes"] = std::move(volumes);
    const auto data = document.dump(4);
    const auto path = getVolumesPersistencePath(mpc);
    const auto temporary = mpc_fs::path(path.string() + ".tmp-" +
                                        std::to_string(std::random_device{}()));
    std::ofstream stream(temporary, std::ios::out | std::ios::binary);
    stream.write(data.data(), static_cast<std::streamsize>(data.size()));
    stream.flush();
    stream.close();
    if (!stream)
    {
        (void)mpc_fs::remove(temporary);
        MLOG("Unable to persist volume settings");
        return false;
    }
    const auto committed = mpc_fs::rename(temporary, path);
    if (!committed)
    {
        (void)mpc_fs::remove(temporary);
    }
    return success(committed, FailurePolicy::BestEffort,
                   "save volume settings");
}
