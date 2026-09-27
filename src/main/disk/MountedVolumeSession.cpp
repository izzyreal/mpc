#include "MountedVolumeSession.hpp"
#include "ImageFileDevice.hpp"
#include "platform/NativeFilePicker.hpp"
#include <ImageBlockDevice.hpp>
#include <FileSystemFactory.hpp>
#include <fat/AkaiFatLfnDirectory.hpp>
#include <util/VolumeMounter.h>
#include <Logger.hpp>

using namespace mpc::disk;

MountedVolumeSession::MountedVolumeSession(const Volume &volume, Open open,
                                           Release release)
    : source(volume.volumePath),
      release(release ? std::move(release)
                      : akaifat::util::VolumeMounter::unmount)
{
    if (volume.type != USB_VOLUME || volume.mode == DISABLED)
    {
        throw std::runtime_error("Device is disabled or unsupported");
    }
    if (!open)
    {
        open = akaifat::util::VolumeMounter::mount;
    }
    try
    {
        accessAcquired = true;
        stream = open(source, volume.mode == READ_ONLY);
        if (!stream.is_open())
        {
            throw std::runtime_error("Unable to mount device");
        }
        device = std::make_shared<akaifat::ImageBlockDevice>(stream,
                                                             volume.volumeSize);
        filesystem.reset(akaifat::fat::AkaiFatFileSystem::read(
            device, volume.mode == READ_ONLY));
        if (!getRoot())
        {
            throw std::runtime_error("Unable to read device root");
        }
    }
    catch (...)
    {
        try
        {
            releaseResources();
        }
        catch (...)
        {
        }
        throw;
    }
}

MountedVolumeSession::~MountedVolumeSession()
{
    try
    {
        close();
    }
    catch (...)
    {
        MLOG("Failed to flush mounted volume at shutdown");
        try
        {
            releaseResources();
        }
        catch (...)
        {
        }
    }
}

std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory>
MountedVolumeSession::getRoot() const
{
    if (!filesystem)
    {
        throw std::runtime_error("Device is not mounted");
    }
    return std::dynamic_pointer_cast<akaifat::fat::AkaiFatLfnDirectory>(
        filesystem->getRoot());
}

bool MountedVolumeSession::isReadOnly() const
{
    if (!filesystem)
    {
        throw std::runtime_error("Device is not mounted");
    }
    return filesystem->isReadOnly();
}

void MountedVolumeSession::flush()
{
    if (!filesystem)
    {
        throw std::runtime_error("Device is not mounted");
    }
    if (filesystem->isReadOnly())
    {
        return;
    }
    filesystem->flush();
    device->flush();
    if (stream.is_open())
    {
        stream.flush();
        if (!stream)
        {
            throw std::runtime_error("Unable to flush device");
        }
    }
}

void MountedVolumeSession::close()
{
    if (!accessAcquired)
    {
        return;
    }
    flush();
    releaseResources();
}

void MountedVolumeSession::releaseResources()
{
    filesystem.reset();
    if (device)
    {
        device->close();
    }
    device.reset();
    if (stream.is_open())
    {
        stream.close();
    }
    if (accessAcquired)
    {
        accessAcquired = false;
        if (release)
        {
            release(source);
        }
    }
}

namespace
{
    void
    validateFat16Geometry(const std::shared_ptr<akaifat::BlockDevice> &device)
    {
        akaifat::ByteBuffer boot(512);
        device->read(0, boot);
        const auto &bytes = boot.getBuffer();
        const auto u8 = [&](int i)
        {
            return static_cast<unsigned char>(bytes[i]);
        };
        const auto u16 = [&](int i)
        {
            return u8(i) | (u8(i + 1) << 8);
        };
        const auto u32 = [&](int i)
        {
            return static_cast<uint64_t>(u16(i)) |
                   (static_cast<uint64_t>(u16(i + 2)) << 16);
        };
        const uint64_t sectors = u16(19) ? u16(19) : u32(32);
        const uint64_t reserved = u16(14), fats = u8(16), fatSectors = u16(22);
        const uint64_t rootEntries = u16(17), clusterSectors = u8(13);
        if (u16(510) != 0xaa55 || u16(11) != 512 || !reserved || fats < 1 ||
            fats > 2 || !fatSectors || !rootEntries || rootEntries % 16 ||
            !clusterSectors || clusterSectors > 128 ||
            (clusterSectors & (clusterSectors - 1)))
        {
            throw std::runtime_error("Unsupported or invalid FAT16 image");
        }
        const uint64_t metadata =
            reserved + fats * fatSectors + rootEntries / 16;
        if (sectors <= metadata ||
            sectors > static_cast<uint64_t>(device->getSize()) / 512)
        {
            throw std::runtime_error("Truncated or invalid FAT16 image");
        }
        const auto clusters = (sectors - metadata) / clusterSectors;
        const bool explicitFat16 =
            std::string(bytes.data() + 54, 8) == "FAT16   ";
        if ((!explicitFat16 && clusters < 4085) || !clusters ||
            clusters > 65524 || (clusters + 2) * 2 > fatSectors * 512)
        {
            throw std::runtime_error(
                "Only FAT16 images supported by the Akai layer are accepted");
        }
        // The existing FAT layer assumes acyclic, in-range chains. Check the
        // allocation graph before handing an externally supplied image to it.
        akaifat::ByteBuffer table(static_cast<int>(fatSectors * 512));
        device->read(reserved * 512, table);
        std::vector<uint16_t> next(clusters + 2);
        for (size_t i = 0; i < next.size(); ++i)
        {
            const auto &data = table.getBuffer();
            next[i] = static_cast<unsigned char>(data[i * 2]) |
                      (static_cast<unsigned char>(data[i * 2 + 1]) << 8);
        }
        std::vector<uint8_t> visited(next.size(), 0);
        for (size_t start = 2; start < next.size(); ++start)
        {
            if (visited[start] || next[start] == 0 || next[start] == 0xfff7)
            {
                continue;
            }
            std::vector<size_t> chain;
            size_t current = start;
            while (true)
            {
                if (current < 2 || current >= next.size() ||
                    visited[current] == 1 || next[current] == 0 ||
                    next[current] == 0xfff7)
                {
                    throw std::runtime_error("Invalid FAT16 allocation chain");
                }
                if (visited[current] == 2)
                {
                    break;
                }
                visited[current] = 1;
                chain.push_back(current);
                const auto target = next[current];
                if (target >= 0xfff8)
                {
                    break;
                }
                current = target;
            }
            for (const auto cluster : chain)
            {
                visited[cluster] = 2;
            }
        }
    }
} // namespace

std::shared_ptr<MountedVolumeSession>
MountedVolumeSession::openImage(const Volume &volume, ImageOpen open)
{
    if (volume.mode == DISABLED)
    {
        throw std::runtime_error("Device is disabled in DISKS");
    }
    auto device =
        open ? open(volume, volume.mode == READ_ONLY)
             : platform::openNativeImage(volume, volume.mode == READ_ONLY);
    if (!device)
    {
        throw std::runtime_error("Unable to open image");
    }
    if (volume.mode == READ_WRITE && device->isReadOnly())
    {
        throw std::runtime_error("Image does not permit read/write access");
    }
    return std::shared_ptr<MountedVolumeSession>(
        new MountedVolumeSession(std::move(device), volume.mode == READ_ONLY));
}

MountedVolumeSession::MountedVolumeSession(
    std::shared_ptr<akaifat::BlockDevice> image, bool readOnly)
    : accessAcquired(true), device(std::move(image))
{
    validateFat16Geometry(device);
    filesystem.reset(akaifat::fat::AkaiFatFileSystem::read(device, readOnly));
    if (!getRoot())
    {
        throw std::runtime_error("Unable to read image root");
    }
}

uint64_t MountedVolumeSession::getSize() const
{
    return device->getSize();
}
