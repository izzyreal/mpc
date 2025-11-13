#include "StdDisk.hpp"

#include <algorithm>

#include "Mpc.hpp"

#include "AkaiFileRenamer.hpp"

#include "disk/MpcFile.hpp"

#include "lcdgui/screens/LoadScreen.hpp"

#include "StrUtil.hpp"

using namespace mpc::disk;
using namespace mpc::file;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

StdDisk::StdDisk(Mpc &mpc) : AbstractDisk(mpc) {}

void StdDisk::close()
{
    volume.close();
}

void StdDisk::flush()
{
    volume.flush();
}

void StdDisk::initFiles()
{
    files.clear();
    allFiles.clear();

    AkaiFileRenamer::renameFilesInDirectory(mpc, getDir()->fs_path);

    auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();

    auto view = loadScreen->view;
    auto dirList = getDir()->listFiles();

    for (auto &f : dirList)
    {
        allFiles.push_back(f);

        if (view != 0 && f->isFile())
        {
            std::string name = f->getName();

            if (f->isFile() && name.find('.') != std::string::npos &&
                name.substr(name.length() - 3) == extensions[view])
            {
                files.push_back(f);
            }
        }
        else
        {
            files.push_back(f);
        }
    }

    std::sort(files.begin(), files.end(),
              [](const std::shared_ptr<MpcFile> &f1,
                 const std::shared_ptr<MpcFile> &f2)
              {
                  return f1->getName() < f2->getName();
              });

    std::stable_partition(files.begin(), files.end(),
                          [](const std::shared_ptr<MpcFile> &f)
                          {
                              return f->isDirectory();
                          });

    initParentFiles();

    std::sort(parentFiles.begin(), parentFiles.end(),
              [](const std::shared_ptr<MpcFile> &f1,
                 const std::shared_ptr<MpcFile> &f2)
              {
                  return f1->getName() < f2->getName();
              });
}

void StdDisk::initParentFiles()
{
    parentFiles.clear();
    if (path.empty())
    {
        return;
    }

    auto temp = getParentDir()->listFiles();

    for (auto &f : temp)
    {
        if (f->isDirectory())
        {
            parentFiles.push_back(f);
        }
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
    return fs::remove_all(f.lock()->fs_path) != 0;
}

bool StdDisk::newFolder(const std::string &newDirName)
{
    std::string copy =
        StrUtil::toUpper(StrUtil::replaceAll(newDirName, ' ', "_"));
    auto new_path = getDir()->fs_path;
    new_path.append(copy);
    return fs::create_directory(new_path);
}

std::shared_ptr<MpcFile> StdDisk::newFile(const std::string &newFileName)
{
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
