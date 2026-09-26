#pragma once

#include <cstdint>
#include <string>

namespace mpc::disk
{

    enum VolumeType
    {
        LOCAL_DIRECTORY,
        DISK_IMAGE,
        USB_VOLUME
    };
    enum MountMode
    {
        DISABLED = 0,
        READ_ONLY = 1,
        READ_WRITE = 2
    };

    struct Volume
    {

        std::string label;
        VolumeType type = LOCAL_DIRECTORY;

        /*
         Used when type == LOCAL_DIRECTORY. Absolute path of a directory
         that serves as a root for a virtual disk device.
         */
        std::string localDirectoryPath;

        /*
         Used when type == DISK_IMAGE. Absolute path of a .img file
         that contains a single FAT16 volume.
         */
        std::string diskImagePath;

        /*
         Used when type == USB_VOLUME. For example /dev/sdb on Linux,
         /dev/disk4 on MacOS and \\.\F: on Windows.
         */
        std::string volumePath;

        // Used when type == DISK_IMAGE || USB_VOLUME
        std::string volumeUUID;
        MountMode mode = DISABLED;
        uint64_t volumeSize = 0;
        std::string typeShortName() const
        {
            switch (type)
            {
                case LOCAL_DIRECTORY:
                    return "DIR";
                case USB_VOLUME:
                    return "USB";
                case DISK_IMAGE:
                    return "IMG";
                default:
                    return " ? ";
            }
        }

        static std::string modeShortName(MountMode m)
        {
            switch (m)
            {
                case DISABLED:
                    return "DISABLED";
                case READ_ONLY:
                    return "READ-ONLY";
                case READ_WRITE:
                    return "READ/WRITE";
                default:
                    return "UNKNOWN";
            }
        }

        std::string modeShortName() const
        {
            return modeShortName(mode);
        }
    };
} // namespace mpc::disk
