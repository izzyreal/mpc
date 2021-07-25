#pragma once

#include <file/Directory.hpp>

#include <string>

namespace mpc::disk {

enum VolumeType { LOCAL_DIRECTORY, DISK_IMAGE, USB_VOLUME };
enum MountMode { DISABLED, READ_ONLY, READ_WRITE };

struct Volume {
 
  std::string label;
  VolumeType type = VolumeType::LOCAL_DIRECTORY;
  
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
    
    
    std::shared_ptr<moduru::file::Directory> getRoot()
    {
        return std::make_shared<moduru::file::Directory>(localDirectoryPath, nullptr);
    }
    
    void close() {}
    void flush() {}
};
}
