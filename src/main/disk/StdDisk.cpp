#include "StdDisk.hpp"

#include <Mpc.hpp>

#include <disk/MpcFile.hpp>

#include <file/AkaiName.hpp>

#include <lcdgui/screens/LoadScreen.hpp>

#include <raw/fat/ShortName.hpp>
#include <raw/fat/ShortNameGenerator.hpp>

#include <lang/StrUtil.hpp>

#include <set>

using namespace moduru::lang;
using namespace moduru::raw::fat;
using namespace mpc::disk;
using namespace mpc::file;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

StdDisk::StdDisk(mpc::Mpc& mpc)
	: AbstractDisk(mpc)
{
}

void StdDisk::close()
{
    volume.close();
}

void StdDisk::flush()
{
    volume.flush();
}

void StdDisk::renameFilesToAkai()
{
	auto dirContent = getDir()->listFiles();
	
	std::vector<std::shared_ptr<MpcFile>> files;
    std::vector<std::shared_ptr<MpcFile>> directories;

	copy_if(dirContent.begin(), dirContent.end(), back_inserter(files), [](const std::shared_ptr<MpcFile> f) { return f->isFile(); });
	copy_if(dirContent.begin(), dirContent.end(), back_inserter(directories), [](const std::shared_ptr<MpcFile> f) { return f->isDirectory(); });

	dirContent.clear();

	std::vector<std::string> allCompatibleNames;

	for (auto& file : files)
    {
		
		auto namesExcludingItself = allCompatibleNames;

		auto itself = find(namesExcludingItself.begin(), namesExcludingItself.end(), file->getName());
		
		if (itself != namesExcludingItself.end())
			namesExcludingItself.erase(itself);

		auto akaiName = AkaiName::generate(file->getName(), namesExcludingItself);
		
		if (akaiName.compare(file->getName()) == 0)
        {
			allCompatibleNames.push_back(akaiName);
			continue;
		}

		allCompatibleNames.push_back(akaiName);
        file->setName(akaiName);
	}
	
	for (auto& dir : directories)
    {
		std::set<std::string> namesAsSet(allCompatibleNames.begin(), allCompatibleNames.end());
		
		auto itself = namesAsSet.find(dir->getName());
		
		if (itself != namesAsSet.end())
			namesAsSet.erase(itself);

		auto shortNameGenerator = ShortNameGenerator(namesAsSet);
		auto akaiName = shortNameGenerator.generateShortName(dir->getName()).asSimpleString();
        
		if (akaiName.compare(dir->getName()) == 0)
        {
			allCompatibleNames.push_back(akaiName);
			continue;
		}

		if (akaiName.find(".") != std::string::npos)
			akaiName = akaiName.substr(0, akaiName.find_last_of("."));

		allCompatibleNames.push_back(akaiName);
		dir->setName(akaiName);
	}
}

void StdDisk::initFiles()
{
	files.clear();
	allFiles.clear();

	renameFilesToAkai();

	auto loadScreen = mpc.screens->get<LoadScreen>("load");

	auto view = loadScreen->view;
	auto dirList = getDir()->listFiles();

	for (auto& f : dirList)
	{
		allFiles.push_back(f);

		if (view != 0 && f->isFile())
		{
			std::string name = f->getName();
		
			if (f->isFile() && name.find(".") != std::string::npos && name.substr(name.length() - 3).compare(extensions[view]) == 0)
				files.push_back(f);
		}
		else {
			files.push_back(f);
		}
	}
    
	initParentFiles();
}

void StdDisk::initParentFiles()
{
	parentFiles.clear();
	if (path.size() == 0) return;

	auto temp = getParentDir()->listFiles();
	
	for (auto& f : temp)
	{
		if (f->isDirectory())
			parentFiles.push_back(f);
	}
}

std::string StdDisk::getDirectoryName()
{
	if (path.size() == 0)
		return "ROOT";
	
	return path[ (int) (path.size()) - 1];
}

bool StdDisk::moveBack()
{
	if (path.size() <= 0)
		return false;

	int lastPathIndex = (int)(path.size()) - 1;

	path.erase(path.begin() + lastPathIndex);
	return true;
}

bool StdDisk::moveForward(const std::string& directoryName)
{
	bool success = false;
	for (auto& f : files)
	{
		if (StrUtil::eqIgnoreCase(StrUtil::trim(f->getName()), StrUtil::trim(directoryName)))
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
	if (path.size() == 0)
        return root;
    
    auto mpcFile = root;
    bool found = false;
    int path_counter = 0;
    
    while (!found) {
        for (auto& f : mpcFile->listFiles())
        {
            if (f->isFile()) continue;
            
            if (f->getName() == path[path_counter])
            {
                mpcFile = f;
                if (path_counter++ == path.size() - 1) found = true;
                break;
            }
        }
    }
    
    return mpcFile;
}

std::shared_ptr<MpcFile> StdDisk::getParentDir()
{
	if (path.size() == 0)
        return {};

	if (path.size() == 1)
        return root;

    auto mpcFile = root;
    bool found = false;
    int path_counter = 0;
    
    while (!found) {
        for (auto& f : mpcFile->listFiles())
        {
            if (f->isFile()) continue;
            
            if (f->getName() == path[path_counter])
            {
                mpcFile = f;
                if (path_counter++ == path.size() - 2) found = true;
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
		return false;

	auto success = false;
	auto files = dir->listFiles();
	
	for (auto& f : files)
	{
		if (!f->isDirectory())
		{
			if (extensionIndex == 0 || StrUtil::hasEnding(f->getName(), extensions[extensionIndex]))
				success = f->del();
		}
	}
	return success;
}

bool StdDisk::newFolder(const std::string& newDirName)
{
    std::string copy = StrUtil::toUpper(StrUtil::replaceAll(newDirName, ' ', "_"));
    auto new_path = getDir()->fs_path;
    new_path.append(copy);
    return fs::create_directory(new_path);
}

std::shared_ptr<MpcFile> StdDisk::newFile(const std::string& newFileName)
{
    std::string copy = StrUtil::toUpper(StrUtil::replaceAll(newFileName, ' ', "_"));
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

int StdDisk::getPathDepth() {
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
