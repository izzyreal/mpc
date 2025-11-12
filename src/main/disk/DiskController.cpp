#include "DiskController.hpp"

#include "Mpc.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/StdDisk.hpp"
#include "disk/RawDisk.hpp"
#include "nvram/VolumesPersistence.hpp"

#include <util/RemovableVolumes.h>

#include "Logger.hpp"

#include <mutex>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::nvram;

using namespace akaifat::util;

DiskController::DiskController(Mpc &_mpc) : mpc(_mpc) {}

void DiskController::initDisks()
{
    disks.emplace_back(std::make_shared<StdDisk>(mpc));
    auto &defaultVolume = disks.back()->getVolume();
    defaultVolume.volumeUUID = "default_volume";
    defaultVolume.type = LOCAL_DIRECTORY;
    defaultVolume.mode = READ_WRITE;
    defaultVolume.label = "DEFAULT";
    defaultVolume.localDirectoryPath =
        mpc.paths->defaultLocalVolumePath().string();
    defaultVolume.volumeSize =
        fs::space(defaultVolume.localDirectoryPath).capacity;
    disks.back()->initRoot();

    MLOG("Disk root initialized");

    detectRawUsbVolumes();

    auto persistedActiveUUID = VolumesPersistence::getPersistedActiveUUID(mpc);
    MLOG("Persisted UUID: " + persistedActiveUUID);

    for (int i = 0; i < disks.size(); i++)
    {
        MLOG("\nIterating through disks: " + std::to_string(i));
        MLOG("Absolute path: " + disks[i]->getAbsolutePath());

        auto uuid = disks[i]->getVolume().volumeUUID;

        MLOG("UUID: " + uuid);

        if (uuid == persistedActiveUUID)
        {
            activeDiskIndex = i;
            break;
        }
    }

    auto activeDisk = getActiveDisk();

    MLOG("Active disk is set to the one with absolute path: " +
         activeDisk->getAbsolutePath());

    if (std::dynamic_pointer_cast<RawDisk>(activeDisk))
    {
        activeDisk->initRoot();

        if (!activeDisk->getVolume().volumeStream.is_open())
        {
            activeDiskIndex = 0;
        }
    }
    else
    {
        MLOG("The active disk is not a raw USB volume.");
    }
}

std::shared_ptr<AbstractDisk> DiskController::getActiveDisk()
{
    if (disks.size() == 0)
    {
        initDisks();
    }

    if (disks.size() == 0 || activeDiskIndex >= disks.size())
    {
        return {};
    }

    return disks[activeDiskIndex];
}

std::vector<std::shared_ptr<AbstractDisk>> &DiskController::getDisks()
{
    return disks;
}

int DiskController::getActiveDiskIndex() const
{
    return activeDiskIndex;
}

void DiskController::setActiveDiskIndex(int newActiveDiskIndex)
{
    activeDiskIndex = newActiveDiskIndex;
}

void DiskController::detectRawUsbVolumes()
{
#ifndef VMPC2000XL_WIN7
    RemovableVolumes removableVolumes;

    MLOG("RemovableVolumes instantiated");

    class SimpleChangeListener : public VolumeChangeListener
    {
    public:
        std::vector<RemovableVolume> volumes;
        std::mutex m;

        void processChange(RemovableVolume v) override
        {
            std::lock_guard lk(m);
            volumes.push_back(std::move(v));
        }

        std::vector<RemovableVolume> snapshot()
        {
            std::lock_guard lk(m);
            return volumes;
        }
    };

    SimpleChangeListener listener;

    MLOG("SimpleChangeListener instantiated");

    removableVolumes.addListener(&listener);

    MLOG("Listener was added to removableVolumes");

    removableVolumes.init();

    MLOG("RemovableVolumes initialized");

    for (int i = 0; i < 10 && listener.volumes.empty(); ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    MLOG("Iterating through scraped USB volumes...");
    auto persistedConfigs = VolumesPersistence::getPersistedConfigs(mpc);

    for (int i = static_cast<int>(disks.size()) - 1; i >= 0; i--)
    {
        auto d = disks[i];

        if (d->getVolume().type == USB_VOLUME)
        {
            bool isConnected = false;

            for (auto &v : listener.volumes)
            {
                if (v.volumeUUID == d->getVolume().volumeUUID)
                {
                    isConnected = true;
                    break;
                }
            }

            if (!isConnected)
            {
                disks.erase(disks.begin() + i);
            }
        }
    }

    for (auto &v : listener.snapshot())
    {
        MLOG("Discovered volume UUID " + v.volumeUUID);

        bool alreadyInitialized = false;

        for (auto &d : disks)
        {
            if (d->getVolume().volumeUUID == v.volumeUUID)
            {
                alreadyInitialized = true;
                break;
            }
        }

        if (alreadyInitialized)
        {
            continue;
        }

        disks.emplace_back(std::make_shared<RawDisk>(mpc));
        auto disk = disks.back();
        auto &volume = disk->getVolume();

        volume.type = USB_VOLUME;

        if (persistedConfigs.find(v.volumeUUID) == end(persistedConfigs))
        {
            volume.mode = DISABLED;
        }
        else
        {
            volume.mode = persistedConfigs[v.volumeUUID];
        }

        volume.volumePath = v.deviceName;
        volume.label = v.volumeName;
        volume.volumeSize = v.mediaSize;
        volume.volumeUUID = v.volumeUUID;
    }

#endif
}
