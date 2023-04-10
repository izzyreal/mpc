#include <disk/RawDisk.hpp>

#include <Mpc.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

#include <disk/MpcFile.hpp>

#include <util/VolumeMounter.h>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>
#include <Logger.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui::screens;
using namespace akaifat::util;
using namespace akaifat::fat;
using namespace moduru::lang;
using namespace moduru::file;

RawDisk::RawDisk(mpc::Mpc& _mpc)
	: AbstractDisk(_mpc)
{
}

RawDisk::~RawDisk()
{
    if (root)
    {
        try
        {
            VolumeMounter::unmount(volume.volumePath);
        }
        catch (const std::exception&)
        {
            MLOG("Failed to unmount " + volume.volumePath + " from VMPC2000XL and mount it back to the host OS!");
        }
    }
}

void RawDisk::initFiles()
{
    files.clear();
    allFiles.clear();

    auto loadScreen = mpc.screens->get<LoadScreen>("load");

    auto view = loadScreen->view;
    auto dirList = getDir()->akaiNameIndex;

    for (auto& f : dirList)
    {
        if ( (f.first.length() > 0 && f.first[0] == '.') || f.first == ".." || f.first == "") continue;
        
        auto mpcFile = std::make_shared<MpcFile>(f.second);
        allFiles.push_back(mpcFile);

        if (view != 0 && mpcFile->isFile())
        {
            std::string name = mpcFile->getName();
        
            if (mpcFile->isFile() && name.find(".") != std::string::npos &&
                name.substr(name.length() - 3).compare(extensions[view]) == 0)
                files.push_back(mpcFile);
        }
        else {
            files.push_back(mpcFile);
        }
    }
    
    initParentFiles();
}

void RawDisk::initParentFiles()
{
	parentFiles.clear();
	if (path.size() == 0) return;

	auto parent = std::dynamic_pointer_cast<akaifat::fat::AkaiFatLfnDirectory>(path[path.size() - 1]->getParent());
    
    for (auto& kv : parent->akaiNameIndex)
	{
		if (kv.first == "." || kv.first == ".." || kv.first.size() == 0)
            continue;
	    if(kv.second->isValid() && kv.second->isDirectory())
		    parentFiles.emplace_back(std::make_shared<MpcFile>(kv.second));
	}
}

std::string RawDisk::getDirectoryName()
{
    if (path.size() == 0) return "ROOT";
    return path[static_cast<int>(path.size()) -1]->getAkaiName();
}

bool RawDisk::moveBack()
{
	if (path.size() == 0) return false;

	path.erase(begin(path) + path.size() - 1);
	return true;
}

bool RawDisk::moveForward(const std::string& directoryName)
{
    std::string dirNameCopy = directoryName;
    std::shared_ptr<AkaiFatLfnDirectoryEntry> entry = std::dynamic_pointer_cast<AkaiFatLfnDirectoryEntry>(getDir()->getEntry(dirNameCopy));
	
    if (!entry || entry->isFile())
        return false;

	path.emplace_back(entry);
	return true;
}

int RawDisk::getPathDepth() {
	return path.size();
}

std::shared_ptr<AkaiFatLfnDirectory> RawDisk::getDir()
{
    if (path.size() == 0)
        return root;

    return std::dynamic_pointer_cast<AkaiFatLfnDirectory>(path[path.size() - 1]->getDirectory());
}

bool RawDisk::deleteAllFiles(int extension)
{
    std::vector<std::shared_ptr<MpcFile>> filesToDelete;

    for (auto& key_value : getDir()->akaiNameIndex)
    {
        auto f = std::make_shared<MpcFile>(key_value.second);

        if (!f->isDirectory())
        {
            if (extension == 0 || StrUtil::hasEnding(f->getName(), extensions[extension]))
            {
                filesToDelete.push_back(f);
            }
        }
    }

    bool success = false;

    for (auto& f : filesToDelete)
    {
        if (f->del()) success = true;
    }

    return success;
}

bool RawDisk::newFolder(const std::string& newDirName)
{
	try {
        std::string copy = newDirName;
		getDir()->addDirectory(copy);
	}
	catch (const std::exception&) {
		return false;
	}
	return true;
}

std::shared_ptr<MpcFile> RawDisk::newFile(const std::string& newFileName)
{
    std::string copy = StrUtil::toUpper(StrUtil::replaceAll(newFileName, ' ', "_"));
    auto newEntry = std::dynamic_pointer_cast<AkaiFatLfnDirectoryEntry>(getDir()->addFile(copy));
    return std::make_shared<MpcFile>(newEntry);
}

std::string RawDisk::getAbsolutePath()
{
	std::string pathString = "";
	
    for (auto& entry : path)
		pathString = pathString + "/" + entry->getAkaiName();
    
	return pathString;
}

void RawDisk::close()
{
    volume.close();
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
