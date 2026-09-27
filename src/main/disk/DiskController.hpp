#pragma once

#include <memory>
#include "MountedVolumeSession.hpp"
#include "platform/NativeFilePicker.hpp"
#include <string>
#include <vector>

namespace mpc
{
    class Mpc;
}
namespace mpc::disk
{
    class AbstractDisk;
}

namespace mpc::disk
{
    class DiskController
    {
        Mpc &mpc;
        bool rawUsbVolumeDetectionEnabled;
        std::vector<std::shared_ptr<AbstractDisk>> disks;
        std::vector<std::string> activeDiskHistory;
        int activeDiskIndex = 0;
        MountedVolumeSession::ImageOpen imageOpen;
        platform::NativeFilePicker filePicker;
        std::string pickerReplaceUuid;

        void initDisks();
        void commitActiveDiskIndex(int);
        std::string activateDiskUnderLease(int index);

    public:
        explicit DiskController(Mpc &, bool rawUsbVolumeDetectionEnabled = true,
                                MountedVolumeSession::ImageOpen = {});
        std::vector<std::shared_ptr<AbstractDisk>> &getDisks();
        std::shared_ptr<AbstractDisk> getActiveDisk();
        int getActiveDiskIndex() const;
        // Empty on success; otherwise the current selection is retained.
        std::string activateDisk(int index);
        bool ensureActiveDiskIsEnabled();

        void detectRawUsbVolumes();
        void setFilePickerParent(void *parent, std::string portalParent = {});
        std::string pickImage(const std::string &replaceUuid = {});
        void pollFilePicker();
        bool isFilePickerPending() const;
        void cancelFilePicker();
        std::string bindImage(Volume, const std::string &replaceUuid = {});
        std::string removeImage(const std::string &uuid);
        std::string validateImage(const std::string &uuid);
        std::string setVolumeMode(const std::string &uuid, MountMode);
    };
} // namespace mpc::disk
