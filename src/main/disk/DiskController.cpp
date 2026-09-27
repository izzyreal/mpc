#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/VmpcDisksScreen.hpp"
#include "DiskController.hpp"

#include "Mpc.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/StdDisk.hpp"
#include "disk/RawDisk.hpp"
#include "nvram/VolumesPersistence.hpp"

#include <util/RemovableVolumes.h>

#include "Logger.hpp"

#include <algorithm>
#include <mutex>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::nvram;

using namespace akaifat::util;

DiskController::DiskController(Mpc &_mpc, bool _rawUsbVolumeDetectionEnabled,
                               MountedVolumeSession::ImageOpen imageOpen)
    : mpc(_mpc), rawUsbVolumeDetectionEnabled(_rawUsbVolumeDetectionEnabled),
      imageOpen(std::move(imageOpen))
{
}

void DiskController::initDisks()
{
    disks.emplace_back(std::make_shared<StdDisk>(mpc));
    auto &defaultVolume = disks.back()->getVolume();
    defaultVolume.volumeUUID = "default_volume";
    defaultVolume.type = LOCAL_DIRECTORY;
    defaultVolume.mode = READ_WRITE;
    defaultVolume.label = "DEFAULT";
    defaultVolume.localDirectoryPath =
        mpc.paths->getDocuments()->defaultLocalVolumePath().string();
    const auto spaceRes = mpc_fs::space(defaultVolume.localDirectoryPath);
    defaultVolume.volumeSize = spaceRes ? spaceRes->capacity : 0;
    disks.back()->initRoot();

    MLOG("Disk root initialized");

    if (rawUsbVolumeDetectionEnabled)
    {
        detectRawUsbVolumes();
    }

    for (const auto &volume : VolumesPersistence::getPersistedImages(mpc))
    {
        auto disk = std::make_shared<RawDisk>(
            mpc,
            [open = imageOpen](const Volume &v)
            {
                return MountedVolumeSession::openImage(v, open);
            });
        disk->getVolume() = volume;
        disks.push_back(disk);
    }

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
            commitActiveDiskIndex(i);
            break;
        }
    }

    ensureActiveDiskIsEnabled();

    auto activeDisk = getActiveDisk();

    MLOG("Active disk is set to the one with absolute path: " +
         activeDisk->getAbsolutePath());

    if (std::dynamic_pointer_cast<RawDisk>(activeDisk))
    {
        try
        {
            activeDisk->initRoot();
        }
        catch (...)
        {
            try
            {
                activeDisk->close();
            }
            catch (...)
            {
            }
            activeDiskIndex = 0;
            activeDiskHistory.clear();
            MLOG("Unable to restore active disk; using DEFAULT");
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

void DiskController::commitActiveDiskIndex(int newActiveDiskIndex)
{
    if (mpc.isManagedSaveActive())
    {
        return;
    }

    if (newActiveDiskIndex == activeDiskIndex)
    {
        return;
    }

    if (activeDiskIndex >= 0 && activeDiskIndex < disks.size() &&
        newActiveDiskIndex >= 0 && newActiveDiskIndex < disks.size())
    {
        const auto &previousUuid =
            disks[activeDiskIndex]->getVolume().volumeUUID;
        const auto &newUuid = disks[newActiveDiskIndex]->getVolume().volumeUUID;

        activeDiskHistory.erase(std::remove(activeDiskHistory.begin(),
                                            activeDiskHistory.end(),
                                            previousUuid),
                                activeDiskHistory.end());
        activeDiskHistory.push_back(previousUuid);
        activeDiskHistory.erase(std::remove(activeDiskHistory.begin(),
                                            activeDiskHistory.end(), newUuid),
                                activeDiskHistory.end());
    }

    activeDiskIndex = newActiveDiskIndex;
}

std::string DiskController::activateDisk(int index)
{
    auto lease = mpc.fileOperationGate.tryAcquire();
    if (!lease)
    {
        return "Disk operation already active";
    }
    return activateDiskUnderLease(index);
}

std::string DiskController::activateDiskUnderLease(int index)
{
    if (index < 0 || index >= disks.size())
    {
        return "Device is unavailable";
    }
    if (disks[index]->getVolume().mode == DISABLED)
    {
        return "Device is disabled in DISKS";
    }
    if (index == activeDiskIndex)
    {
        return {};
    }

    const auto candidate = disks[index];
    const auto previous = activeDiskIndex >= 0 && activeDiskIndex < disks.size()
                              ? disks[activeDiskIndex]
                              : nullptr;
    try
    {
        candidate->initRoot();
        if (previous)
        {
            previous->close();
        }
    }
    catch (const std::exception &e)
    {
        try
        {
            candidate->close();
        }
        catch (...)
        {
            MLOG("Failed to release candidate disk after activation failure");
        }
        return e.what();
    }
    catch (...)
    {
        try
        {
            candidate->close();
        }
        catch (...)
        {
        }
        return "Unable to activate device";
    }
    commitActiveDiskIndex(index);
    return {};
}

bool DiskController::ensureActiveDiskIsEnabled()
{
    if (mpc.isManagedSaveActive())
    {
        return false;
    }

    if (disks.empty())
    {
        return false;
    }

    const auto activeDiskIndexIsValid =
        activeDiskIndex >= 0 && activeDiskIndex < disks.size();

    if (activeDiskIndexIsValid &&
        disks[activeDiskIndex]->getVolume().mode != DISABLED)
    {
        return false;
    }

    while (!activeDiskHistory.empty())
    {
        const auto previousUuid = activeDiskHistory.back();
        activeDiskHistory.pop_back();

        for (int i = 0; i < disks.size(); ++i)
        {
            const auto &volume = disks[i]->getVolume();
            if (volume.volumeUUID == previousUuid && volume.mode != DISABLED)
            {
                if (activateDisk(i).empty())
                {
                    return true;
                }
            }
        }
    }

    for (int i = 0; i < disks.size(); ++i)
    {
        if (disks[i]->getVolume().mode != DISABLED)
        {
            if (activateDisk(i).empty())
            {
                return true;
            }
        }
    }

    return false;
}

void DiskController::detectRawUsbVolumes()
{
    if (mpc.isManagedSaveActive())
    {
        return;
    }

#ifndef VMPC2000XL_WIN7
    if (!rawUsbVolumeDetectionEnabled)
    {
        return;
    }

    const auto activeDiskUuid =
        activeDiskIndex >= 0 && activeDiskIndex < disks.size()
            ? disks[activeDiskIndex]->getVolume().volumeUUID
            : std::string();

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

    const auto activeDisk =
        std::find_if(disks.begin(), disks.end(),
                     [&activeDiskUuid](const auto &d)
                     {
                         return d->getVolume().volumeUUID == activeDiskUuid;
                     });
    activeDiskIndex =
        activeDisk == disks.end()
            ? -1
            : static_cast<int>(std::distance(disks.begin(), activeDisk));

    ensureActiveDiskIsEnabled();

#endif
}

std::string DiskController::bindImage(Volume volume,
                                      const std::string &replaceUuid)
{
    // Consumers may bind directly before any LOAD/SAVE screen initializes disks.
    (void)getActiveDisk();
    auto lease = mpc.fileOperationGate.tryAcquire();
    if (!lease)
    {
        return "Disk operation already active";
    }
    if (volume.mode < DISABLED || volume.mode > READ_WRITE)
    {
        return "Invalid image access mode";
    }
    if (volume.volumeUUID.empty() || volume.diskImagePath.empty())
    {
        return "Invalid image binding";
    }
    auto existing =
        std::find_if(disks.begin(), disks.end(),
                     [&](const auto &d)
                     {
                         return d->getVolume().volumeUUID == replaceUuid;
                     });
    if (!replaceUuid.empty() && (existing == disks.end() ||
                                 (*existing)->getVolume().type != DISK_IMAGE))
    {
        return "Image binding is unavailable";
    }
    if (existing != disks.end() && *existing == getActiveDisk())
    {
        return "Select another device before replacing this image";
    }
    for (const auto &d : disks)
    {
        const auto &v = d->getVolume();
        if (v.volumeUUID != replaceUuid &&
            (v.volumeUUID == volume.volumeUUID ||
             (v.type == DISK_IMAGE && v.diskImagePath == volume.diskImagePath)))
        {
            return "Image is already bound";
        }
    }
    try
    {
        volume.type = DISK_IMAGE;
        auto probe = volume;
        probe.mode = READ_ONLY;
        auto session = MountedVolumeSession::openImage(probe, imageOpen);
        volume.volumeSize = session->getSize();
        session->close();
        if (existing != disks.end())
        {
            volume.volumeUUID = replaceUuid;
            const auto previous = (*existing)->getVolume();
            (*existing)->getVolume() = volume;
            if (!VolumesPersistence::save(mpc))
            {
                (*existing)->getVolume() = previous;
                return "Unable to save image binding";
            }
        }
        else
        {
            auto disk = std::make_shared<RawDisk>(
                mpc,
                [open = imageOpen](const Volume &v)
                {
                    return MountedVolumeSession::openImage(v, open);
                });
            disk->getVolume() = volume;
            disks.push_back(disk);
            if (!VolumesPersistence::save(mpc))
            {
                disks.pop_back();
                return "Unable to save image binding";
            }
        }
        return {};
    }
    catch (const std::exception &e)
    {
        return e.what();
    }
    catch (...)
    {
        return "Invalid or unsupported image";
    }
}

std::string DiskController::removeImage(const std::string &uuid)
{
    auto lease = mpc.fileOperationGate.tryAcquire();
    if (!lease)
    {
        return "Disk operation already active";
    }
    const auto it = std::find_if(disks.begin(), disks.end(),
                                 [&](const auto &d)
                                 {
                                     return d->getVolume().volumeUUID == uuid &&
                                            d->getVolume().type == DISK_IMAGE;
                                 });
    if (it == disks.end())
    {
        return "Image binding is unavailable";
    }
    if (*it == getActiveDisk())
    {
        return "Select another device before removing this binding";
    }
    auto active = getActiveDisk();
    const auto index = std::distance(disks.begin(), it);
    auto removed = *it;
    disks.erase(it);
    activeDiskIndex = std::distance(
        disks.begin(), std::find(disks.begin(), disks.end(), active));
    if (!VolumesPersistence::save(mpc))
    {
        disks.insert(disks.begin() + index, removed);
        activeDiskIndex = std::distance(
            disks.begin(), std::find(disks.begin(), disks.end(), active));
        return "Unable to save image bindings";
    }
    return {};
}

std::string DiskController::validateImage(const std::string &uuid)
{
    auto lease = mpc.fileOperationGate.tryAcquire();
    if (!lease)
    {
        return "Disk operation already active";
    }
    for (const auto &d : disks)
    {
        if (d->getVolume().volumeUUID != uuid ||
            d->getVolume().type != DISK_IMAGE)
        {
            continue;
        }
        try
        {
            if (d == getActiveDisk())
            {
                d->initRoot();
                d->captureSaveDestination(0)->scan();
            }
            else
            {
                auto probe = d->getVolume();
                probe.mode = READ_ONLY;
                auto session =
                    MountedVolumeSession::openImage(probe, imageOpen);
                d->getVolume().volumeSize = session->getSize();
                session->close();
            }
            return {};
        }
        catch (const std::exception &e)
        {
            return e.what();
        }
        catch (...)
        {
            return "Invalid or unsupported image";
        }
    }
    return "Image binding is unavailable";
}

std::string DiskController::setVolumeMode(const std::string &uuid,
                                          MountMode mode)
{
    if (mode < DISABLED || mode > READ_WRITE)
    {
        return "Invalid access mode";
    }
    for (const auto &d : disks)
    {
        auto &volume = d->getVolume();
        if (volume.volumeUUID != uuid)
        {
            continue;
        }
        if (uuid == "default_volume" || volume.mode == mode)
        {
            return {};
        }
        auto lease = mpc.fileOperationGate.tryAcquire();
        if (!lease)
        {
            return "Disk operation already active";
        }
        const bool wasActive = d == getActiveDisk();
        const int previousIndex = activeDiskIndex;
        const auto previousHistory = activeDiskHistory;
        if (wasActive)
        {
            // Release the old access before reopening with the new policy.
            // Keep a usable fallback active if reopening fails.
            int fallback = 0;
            for (auto history = activeDiskHistory.rbegin();
                 history != activeDiskHistory.rend(); ++history)
            {
                const auto found = std::find_if(
                    disks.begin(), disks.end(),
                    [&](const auto &candidate)
                    {
                        return candidate != d &&
                               candidate->getVolume().volumeUUID == *history &&
                               candidate->getVolume().mode != DISABLED;
                    });
                if (found != disks.end())
                {
                    fallback = std::distance(disks.begin(), found);
                    break;
                }
            }
            const auto error = activateDiskUnderLease(fallback);
            if (!error.empty())
            {
                return error;
            }
        }
        volume.mode = mode;
        if (wasActive && mode != DISABLED)
        {
            const auto error = activateDiskUnderLease(previousIndex);
            if (!error.empty())
            {
                return error;
            }
            // A permission change is not a user selection change.
            activeDiskHistory = previousHistory;
        }
        return {};
    }
    return "Device is unavailable";
}

void DiskController::setFilePickerParent(void *parent, std::string portalParent)
{
    filePicker.setParent(parent, std::move(portalParent));
}
bool DiskController::isFilePickerPending() const
{
    return filePicker.pending();
}
void DiskController::cancelFilePicker()
{
    filePicker.cancel();
}
std::string DiskController::pickImage(const std::string &replaceUuid)
{
    auto lease = mpc.fileOperationGate.tryAcquire();
    if (!lease)
    {
        return "Disk operation already active";
    }
    if (!filePicker.request())
    {
        return "File picker already open";
    }
    pickerReplaceUuid = replaceUuid;
    return {};
}
void DiskController::pollFilePicker()
{
    auto result = filePicker.poll();
    if (!result)
    {
        return;
    }
    std::string error = result->error;
    if (result->image)
    {
        if (!pickerReplaceUuid.empty())
        {
            for (const auto &disk : disks)
            {
                if (disk->getVolume().volumeUUID == pickerReplaceUuid)
                {
                    result->image->mode = disk->getVolume().mode;
                }
            }
        }
        error = bindImage(std::move(*result->image), pickerReplaceUuid);
    }
    pickerReplaceUuid.clear();
    if (result->image && error.empty())
    {
        mpc.screens->get<ScreenId::VmpcDisksScreen>()->refreshConfig();
        mpc.getLayeredScreen()->showPopupForMs("Image binding saved", 1500);
    }
    else if (!error.empty())
    {
        MLOG("Image selection failed: " + error);
        mpc.getLayeredScreen()->showPopupForMs(error, 2500);
    }
}
