#include "StdDisk.hpp"

#include <algorithm>

#include "Mpc.hpp"

#include "AkaiFileRenamer.hpp"

#include "FileIoPolicy.hpp"
#include "disk/MpcFile.hpp"

#include "lcdgui/screens/LoadScreen.hpp"

#include "StrUtil.hpp"
#include <Logger.hpp>

using namespace mpc::disk;
using namespace mpc::file;
using namespace mpc::file_io;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

StdDisk::StdDisk(Mpc &mpc) : AbstractDisk(mpc) {}

void StdDisk::close()
{
    if (isSaveBusy())
    {
        return;
    }

    volume.close();
}

void StdDisk::flush()
{
    volume.flush();
}

void StdDisk::initFiles()
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

std::string StdDisk::getDirectoryName()
{
    if (path.empty())
    {
        return "ROOT";
    }

    return path[(int)path.size() - 1];
}

bool StdDisk::moveBack()
{
    if (isSaveBusy())
    {
        return false;
    }

    if (path.empty())
    {
        return false;
    }

    int lastPathIndex = (int)path.size() - 1;

    path.erase(path.begin() + lastPathIndex);
    return true;
}

bool StdDisk::moveForward(const std::string &directoryName)
{
    if (isSaveBusy())
    {
        return false;
    }

    bool success = false;
    for (auto &f : files)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::trim(f->getName()),
                                  StrUtil::trim(directoryName)))
        {
            path.push_back(f->getName());
            success = true;
            break;
        }
    }

    return success;
}

std::shared_ptr<MpcFile> StdDisk::getDir()
{
    if (path.empty())
    {
        return root;
    }

    auto mpcFile = root;
    bool found = false;
    int path_counter = 0;

    while (!found)
    {
        for (auto &f : mpcFile->listFiles())
        {
            if (f->isFile())
            {
                continue;
            }

            if (f->getName() == path[path_counter])
            {
                mpcFile = f;
                if (path_counter++ == path.size() - 1)
                {
                    found = true;
                }
                break;
            }
        }
    }

    return mpcFile;
}

std::shared_ptr<MpcFile> StdDisk::getParentDir()
{
    if (path.empty())
    {
        return {};
    }

    if (path.size() == 1)
    {
        return root;
    }

    auto mpcFile = root;
    bool found = false;
    int path_counter = 0;

    while (!found)
    {
        for (auto &f : mpcFile->listFiles())
        {
            if (f->isFile())
            {
                continue;
            }

            if (f->getName() == path[path_counter])
            {
                mpcFile = f;
                if (path_counter++ == path.size() - 2)
                {
                    found = true;
                }
                break;
            }
        }
    }

    return mpcFile;
}

bool StdDisk::deleteAllFiles(int extensionIndex)
{
    if (isSaveBusy())
    {
        return false;
    }

    auto dir = getDir();

    if (!dir)
    {
        return false;
    }

    auto success = false;
    auto files = dir->listFiles();

    for (auto &f : files)
    {
        if (!f->isDirectory())
        {
            if (extensionIndex == 0 ||
                StrUtil::hasEnding(f->getName(), extensions[extensionIndex]))
            {
                success = f->del();
            }
        }
    }
    return success;
}

bool StdDisk::deleteRecursive(std::weak_ptr<MpcFile> f)
{
    const auto locked = f.lock();
    if (!locked)
    {
        return false;
    }

    const auto removedCount =
        value(mpc_fs::remove_all(locked->fs_path), FailurePolicy::Required,
              "recursive folder deletion");
    return removedCount && *removedCount != 0;
}

bool StdDisk::newFolder(const std::string &newDirName)
{
    if (isSaveBusy())
    {
        return false;
    }

    std::string copy =
        StrUtil::toUpper(StrUtil::replaceAll(newDirName, ' ', "_"));
    auto new_path = getDir()->fs_path;
    new_path.append(copy);
    return success(mpc_fs::create_directory(new_path), FailurePolicy::Required,
                   "folder creation");
}

std::shared_ptr<MpcFile> StdDisk::newFile(const std::string &newFileName)
{
    if (isSaveBusy())
    {
        throw std::runtime_error("Disk operation already active");
    }

    std::string copy =
        StrUtil::toUpper(StrUtil::replaceAll(newFileName, ' ', "_"));
    auto new_path = getDir()->fs_path;
    new_path.append(copy);
    auto result = std::make_shared<MpcFile>(new_path);
    result->getOutputStream();
    return result;
}

std::string StdDisk::getAbsolutePath()
{
    return getDir()->fs_path.string();
}

int StdDisk::getPathDepth()
{
    return path.size();
}

std::string StdDisk::getTypeShortName()
{
    return volume.typeShortName();
}

uint64_t StdDisk::getTotalSize()
{
    return volume.volumeSize;
}

std::string StdDisk::getVolumeLabel()
{
    return volume.label;
}

std::unique_ptr<SaveDestination> StdDisk::captureSaveDestination(int view)
{
    if (!root)
    {
        throw std::runtime_error("No save destination");
    }
    // Capture names only. getDir() walks the filesystem and can loop forever
    // when a previously selected directory has disappeared.
    auto location = root->getPath();
    for (const auto &component : path)
    {
        location /= component;
    }
    const auto directory = std::make_shared<MpcFile>(location);
    const auto parent = path.empty()
                            ? std::shared_ptr<MpcFile>{}
                            : std::make_shared<MpcFile>(location.parent_path());
    const auto tempRoot = mpc.paths->getDocuments()->tempPath();
    return std::make_unique<SaveDestination>(
        [directory]
        {
            return directory->listFilesChecked();
        },
        [parent]
        {
            return parent ? parent->listFilesChecked()
                          : std::vector<std::shared_ptr<MpcFile>>{};
        },
        [directory](const std::string &name)
        {
            return directory->createChildFileChecked(name);
        },
        true, volume.mode == READ_WRITE, view,
        [location, tempRoot]
        {
            AkaiFileRenamer::renameFilesInDirectory(location, tempRoot);
        },
        [] {});
}
