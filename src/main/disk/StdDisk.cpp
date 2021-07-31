#include "StdDisk.hpp"

#include <Mpc.hpp>

#include <disk/MpcFile.hpp>
#include <disk/Volume.hpp>

#include <file/AkaiName.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

#include <raw/fat/ShortName.hpp>
#include <raw/fat/ShortNameGenerator.hpp>

#include <lang/StrUtil.hpp>

#include <file/FsNode.hpp>
#include <file/Directory.hpp>
#include <file/FileUtil.hpp>

#include <set>

using namespace moduru::lang;
using namespace moduru::file;
using namespace moduru::raw::fat;
using namespace mpc::disk;
using namespace mpc::file;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

StdDisk::StdDisk(mpc::Mpc& mpc, Volume& _volume)
	: AbstractDisk(mpc), volume (_volume), root (_volume.getRoot())
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
	
	vector<shared_ptr<MpcFile>> files;
	vector<shared_ptr<MpcFile>> directories;

	copy_if(dirContent.begin(), dirContent.end(), back_inserter(files), [](const shared_ptr<MpcFile> f) { return f->isFile(); });
	copy_if(dirContent.begin(), dirContent.end(), back_inserter(directories), [](const shared_ptr<MpcFile> f) { return f->isDirectory(); });

	dirContent.clear();

	vector<string> allCompatibleNames;

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
		set<string> namesAsSet(allCompatibleNames.begin(), allCompatibleNames.end());
		
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

		if (akaiName.find(".") != string::npos)
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
			string name = f->getName();
		
			if (f->isFile() && name.find(".") != string::npos && name.substr(name.length() - 3).compare(extensions[view]) == 0)
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

string StdDisk::getDirectoryName()
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

bool StdDisk::moveForward(const string& directoryName)
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

    string dirPath = root->stdNode->getPath();
    
    for (int i = 0; i < path.size(); i++)
        dirPath = dirPath + FileUtil::getSeparator() + path[i];
    
	return std::make_shared<MpcFile>(std::make_shared<Directory>(dirPath, nullptr));
}

std::shared_ptr<MpcFile> StdDisk::getParentDir()
{
	if (path.size() == 0)
        return {};

	if (path.size() == 1)
        return root;

    string dirPath = root->stdNode->getPath();
    
    for (int i = 0; i < path.size() - 1; i++)
    {
        dirPath = dirPath + FileUtil::getSeparator() + path[i];
    }
    
    return std::make_shared<MpcFile>(std::make_shared<Directory>(dirPath, nullptr));
}

bool StdDisk::deleteAllFiles(int extension)
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
			if (extension == 0 || StrUtil::hasEnding(f->getName(), extensions[extension]))
				success = f->del();
		}
	}
	return success;
}

bool StdDisk::newFolder(const string& newDirName)
{
    auto dir = getDir();
	auto f = Directory(dir->stdNode->getPath() + FileUtil::getSeparator() + StrUtil::toUpper(newDirName), std::dynamic_pointer_cast<Directory>(dir->stdNode));
	return f.create();
}

bool StdDisk::deleteDir(weak_ptr<MpcFile> f)
{
    return deleteRecursive(f);
}

bool StdDisk::deleteRecursive(std::weak_ptr<MpcFile> _toDelete)
{
    auto toDelete = _toDelete.lock();
    
	if (toDelete->isDirectory())
	{
		for (auto& f : toDelete->listFiles())
			deleteRecursive(f);
	}
	
	return toDelete->del();
}

shared_ptr<MpcFile> StdDisk::newFile(const string& _newFileName)
{
    shared_ptr<File> f;
    
    auto fileName = _newFileName;
    
	try
    {
		auto split = FileUtil::splitName(fileName);
		split[0] = moduru::lang::StrUtil::trim(split[0]);
		fileName = split[0] + "." + split[1];

        auto dir = getDir();
        
		string filePath = dir->stdNode->getPath() + FileUtil::getSeparator() + StrUtil::toUpper(StrUtil::replaceAll(fileName, ' ', "_"));
        
		f = make_shared<File>(filePath, std::dynamic_pointer_cast<Directory>(dir->stdNode));
		auto success = f->create();
		
        if (success)
        {
			return make_shared<MpcFile>(f);
		}
	}
	catch (const exception& e) {
        MLOG("Failed to create file " + _newFileName + ":");
        string msg = e.what();
        MLOG(msg);
	}
    
	return {};
}

string StdDisk::getAbsolutePath()
{
    return getDir()->stdNode->getPath();
}

int StdDisk::getPathDepth() {
	return path.size();
}
