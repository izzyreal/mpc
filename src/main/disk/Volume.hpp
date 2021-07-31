#pragma once

#include <disk/MpcFile.hpp>
#include <file/Directory.hpp>

#include <ImageBlockDevice.hpp>
#include <FileSystemFactory.hpp>
#include <fat/AkaiFatFileSystem.hpp>
#include <fat/AkaiFatLfnDirectory.hpp>
#include <util/VolumeMounter.h>

#include <string>

namespace mpc::disk {

enum VolumeType { LOCAL_DIRECTORY, DISK_IMAGE, USB_VOLUME };
enum MountMode { DISABLED = 0, READ_ONLY = 1, READ_WRITE = 2 };

struct Volume {
    
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
    MountMode mode = MountMode::DISABLED;
    uint64_t volumeSize;
    
    std::string typeShortName()
    {
        switch (type)
        {
            case LOCAL_DIRECTORY: return "DIR";
            case USB_VOLUME:      return "USB";
            case DISK_IMAGE:      return "IMG";
            default:              return " ? ";
        }
    }
    
    std::string modeShortName()
    {
        switch (mode)
        {
            case DISABLED:    return "DISABLED";
            case READ_ONLY:   return "READ-ONLY";
            case READ_WRITE:  return "READ/WRITE";
            default:          return "UNKNOWN";
        }
    }
    
    std::string sizeShortName()
    {
        
    }
    
    std::shared_ptr<mpc::disk::MpcFile> getRoot()
    {
        if (type == LOCAL_DIRECTORY)
            return std::make_shared<mpc::disk::MpcFile>(std::make_shared<moduru::file::Directory>(localDirectoryPath, nullptr));
        else
            return {};
    }
    
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> getRawRoot()
    {
        if (type == USB_VOLUME)
        {
            auto volumeStream = akaifat::util::VolumeMounter::mount(volumePath, false);
         
            if (volumeStream.is_open())
            {
                auto device = std::make_shared<akaifat::ImageBlockDevice>(volumeStream, volumeSize);
                auto fs = dynamic_cast<akaifat::fat::AkaiFatFileSystem *>(akaifat::FileSystemFactory::createAkai(device, false));

                return std::dynamic_pointer_cast<akaifat::fat::AkaiFatLfnDirectory>(fs->getRoot());
            }
        }
        return {};
    }
    
    void close() {}
    void flush() {}
};
}
