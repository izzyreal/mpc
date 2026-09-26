#include "disk/RawDisk.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include "disk/MpcFile.hpp"

#include <util/VolumeMounter.h>

#include "StrUtil.hpp"
#include <Logger.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui::screens;
using namespace akaifat::util;
using namespace akaifat::fat;
using namespace mpc::lcdgui;

RawDisk::RawDisk(Mpc &_mpc) : AbstractDisk(_mpc) {}

RawDisk::~RawDisk()
{
    if (root)
    {
        try
        {
            VolumeMounter::unmount(volume.volumePath);
        }
        catch (const std::exception &)
        {
            MLOG("Failed to unmount " + volume.volumePath +
                 " from VMPC2000XL and mount it back to the host OS!");
        }
    }
}

void RawDisk::initFiles()
{
    if (mpc.isManagedSaveActive() || consumePreparedListing())
    {
        return;
    }
    auto destination =
        captureSaveDestination(mpc.screens->get<ScreenId::LoadScreen>()->view);
    try
    {
        publishListing(destination->scan());
    }
    catch (const std::exception &e)
    {
        MLOG(std::string("Directory refresh failed: ") + e.what());
        publishListing({});
    }
}

std::string RawDisk::getDirectoryName()
{
    if (path.size() == 0)
    {
        return "ROOT";
    }
    return path[static_cast<int>(path.size()) - 1]->getAkaiName();
}

bool RawDisk::moveBack()
{
    if (isSaveBusy())
    {
        return false;
    }

    if (path.size() == 0)
    {
        return false;
    }

    path.erase(begin(path) + path.size() - 1);
    return true;
}

bool RawDisk::moveForward(const std::string &directoryName)
{
    if (isSaveBusy())
    {
        return false;
    }

    std::string dirNameCopy = directoryName;
    std::shared_ptr<AkaiFatLfnDirectoryEntry> entry =
        std::dynamic_pointer_cast<AkaiFatLfnDirectoryEntry>(
            getDir()->getEntry(dirNameCopy));

    if (!entry || entry->isFile())
    {
        return false;
    }

    path.emplace_back(entry);
    return true;
}

int RawDisk::getPathDepth()
{
    return path.size();
}

std::shared_ptr<AkaiFatLfnDirectory> RawDisk::getDir()
{
    if (path.size() == 0)
    {
        return root;
    }

    return std::dynamic_pointer_cast<AkaiFatLfnDirectory>(
        path[path.size() - 1]->getDirectory());
}

bool RawDisk::deleteAllFiles(int extension)
{
    if (isSaveBusy())
    {
        return false;
    }

    std::vector<std::shared_ptr<MpcFile>> filesToDelete;

    for (auto &key_value : getDir()->akaiNameIndex)
    {
        auto f = std::make_shared<MpcFile>(key_value.second);

        if (!f->isDirectory())
        {
            if (extension == 0 ||
                StrUtil::hasEnding(f->getName(), extensions[extension]))
            {
                filesToDelete.push_back(f);
            }
        }
    }

    bool success = false;

    for (auto &f : filesToDelete)
    {
        if (f->del())
        {
            success = true;
        }
    }

    return success;
}

bool RawDisk::newFolder(const std::string &newDirName)
{
    if (isSaveBusy())
    {
        return false;
    }

    try
    {
        std::string copy = newDirName;
        getDir()->addDirectory(copy);
    }
    catch (const std::exception &)
    {
        return false;
    }
    return true;
}

std::shared_ptr<MpcFile> RawDisk::newFile(const std::string &newFileName)
{
    if (isSaveBusy())
    {
        throw std::runtime_error("Disk operation already active");
    }

    std::string copy =
        StrUtil::toUpper(StrUtil::replaceAll(newFileName, ' ', "_"));
    auto newEntry = std::dynamic_pointer_cast<AkaiFatLfnDirectoryEntry>(
        getDir()->addFile(copy));
    return std::make_shared<MpcFile>(newEntry);
}

std::string RawDisk::getAbsolutePath()
{
    std::string pathString = "";

    for (auto &entry : path)
    {
        pathString = pathString + "/" + entry->getAkaiName();
    }

    return pathString;
}

void RawDisk::close()
{
    if (isSaveBusy())
    {
        return;
    }

    const bool shouldUnmount = root != nullptr;

    if (volume.volumeStream.is_open())
    {
        volume.close();
        volume.volumeFs = nullptr;
        volume.volumeDevice = {};
    }

    if (shouldUnmount)
    {
        try
        {
            VolumeMounter::unmount(volume.volumePath);
        }
        catch (const std::exception &)
        {
            MLOG("Failed to unmount " + volume.volumePath +
                 " from VMPC2000XL and mount it back to the host OS!");
        }
    }

    root = {};
    path.clear();
    files.clear();
    allFiles.clear();
    parentFiles.clear();
}

void RawDisk::flush()
{
    volume.flush();
}

std::string RawDisk::getTypeShortName()
{
    return volume.typeShortName();
}

uint64_t RawDisk::getTotalSize()
{
    return volume.volumeSize;
}

std::string RawDisk::getVolumeLabel()
{
    return volume.label;
}

std::unique_ptr<SaveDestination> RawDisk::captureSaveDestination(int view)
{
    const auto directory = getDir();
    const auto parent = path.empty()
                            ? std::shared_ptr<AkaiFatLfnDirectory>{}
                            : std::dynamic_pointer_cast<AkaiFatLfnDirectory>(
                                  path.back()->getParent());
    return SaveDestination::fromRaw(
        directory, parent, volume.mode == READ_WRITE, view,
        [this]
        {
            flush();
            if (!volume.volumeStream)
            {
                throw std::runtime_error("Unable to flush save volume");
            }
        });
}
