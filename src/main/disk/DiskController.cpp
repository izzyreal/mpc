#include "DiskController.hpp"

#include <Mpc.hpp>
#include <Paths.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/RawDisk.hpp>
#include <disk/Volume.hpp>
#include <nvram/VolumesPersistence.hpp>

#include <file/FileUtil.hpp>

#include <util/RemovableVolumes.h>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::nvram;

using namespace akaifat::util;

DiskController::DiskController(mpc::Mpc& _mpc)
: mpc (_mpc)
{
}

void DiskController::initDisks()
{
    auto persistedConfigs = VolumesPersistence::getPersistedConfigs();
    auto persistedActiveUUID = VolumesPersistence::getPersistedActiveUUID();
    
    disks.emplace_back(std::make_shared<StdDisk>(mpc));
    auto& defaultVolume = disks.back()->getVolume();
    defaultVolume.volumeUUID = "default_volume";
    defaultVolume.type = LOCAL_DIRECTORY;
    defaultVolume.mode = READ_WRITE;
    defaultVolume.label = "DEFAULT";
    defaultVolume.localDirectoryPath = mpc::Paths::defaultLocalVolumePath();
#ifdef _WIN32
    defaultVolume.volumeSize = moduru::file::FileUtil::getTotalDiskSpace(defaultVolume.localDirectoryPath.substr(0, 1));
#else
    defaultVolume.volumeSize = moduru::file::FileUtil::getTotalDiskSpace();
#endif
    disks.back()->initRoot();
    
    MLOG("Disk root initialized");
    
#ifndef VMPC2000XL_WIN7

    RemovableVolumes removableVolumes;
    
    MLOG("RemovableVolumes instantiated");

    class SimpleChangeListener : public VolumeChangeListener {
    public:
        std::vector<RemovableVolume> volumes;
        void processChange(RemovableVolume v) override {
            volumes.push_back(v);
        }
    };
    
    SimpleChangeListener listener;
    
    MLOG("SimpleChangeListener instantiated");

    removableVolumes.addListener(&listener);
    
    MLOG("Listener was added to removableVolumes");

    removableVolumes.init();
    
    MLOG("RemovableVolumes initialized");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    MLOG("Iterating through scraped USB volumes...");

    for (auto& v : listener.volumes)
    {
        MLOG("Discovered volume UUID " + v.volumeUUID);
        disks.emplace_back(std::make_shared<RawDisk>(mpc));
        auto disk = disks.back();
        auto& volume = disk->getVolume();
        
        volume.type = USB_VOLUME;
        
        if (persistedConfigs.find(v.volumeUUID) == end(persistedConfigs))
            volume.mode = DISABLED;
        else
            volume.mode = persistedConfigs[v.volumeUUID];
        
        volume.volumePath = v.deviceName;
        volume.label = v.volumeName;
        volume.volumeSize = v.mediaSize;
        volume.volumeUUID = v.volumeUUID;
    }

#endif

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
    
    auto activeDisk = getActiveDisk().lock();

    MLOG("Active disk is set to the one with absolute path: " + activeDisk->getAbsolutePath());

    if (std::dynamic_pointer_cast<RawDisk>(getActiveDisk().lock()))
    {
        getActiveDisk().lock()->initRoot();

        if (!getActiveDisk().lock()->getVolume().volumeStream.is_open())
        {
            activeDiskIndex = 0;
        }
    }
    else
    {
        MLOG("The active disk is not a raw USB volume.");
    }
}

std::weak_ptr<AbstractDisk> DiskController::getActiveDisk()
{
    return disks[activeDiskIndex];
}

std::vector<std::shared_ptr<AbstractDisk>>& DiskController::getDisks()
{
    return disks;
}
