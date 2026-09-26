#include "MountedVolumeSession.hpp"
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
    stream.flush();
    if (!stream)
    {
        throw std::runtime_error("Unable to flush device");
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
    device.reset();
    if (stream.is_open())
    {
        stream.close();
    }
    if (accessAcquired)
    {
        accessAcquired = false;
        release(source);
    }
}
