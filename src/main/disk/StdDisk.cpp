#include <disk/StdDisk.hpp>

#include <Mpc.hpp>

#include <disk/MpcFile.hpp>
#include <disk/Store.hpp>
#include <disk/device/Device.hpp>
#include <disk/device/StdDevice.hpp>
#include <ui/disk/DiskGui.hpp>
#include <file/AkaiName.hpp>

#include <raw/fat/ShortName.hpp>
#include <raw/fat/ShortNameGenerator.hpp>

#include <lang/StrUtil.hpp>

#include <file/FsNode.hpp>
#include <file/Directory.hpp>
#include <file/FileUtil.hpp>


using namespace moduru::lang;
using namespace moduru::file;
using namespace mpc::disk;
using namespace mpc::file;
using namespace std;

StdDisk::StdDisk(weak_ptr<Store> store, mpc::Mpc* mpc)
	: AbstractDisk(store, mpc)
{
	device = make_unique<mpc::disk::device::StdDevice>(store.lock()->path);
	if (device) {
		root = nonstd::any_cast<weak_ptr<Directory>>(device->getRoot());
		initFiles();
	}
}

void StdDisk::renameFilesToAkai() {

	auto dirContent = getDir().lock()->listFiles(true);
	
	vector<shared_ptr<FsNode>> files;
	vector<shared_ptr<FsNode>> directories;

	copy_if(dirContent.begin(), dirContent.end(), back_inserter(files), [](const shared_ptr<FsNode> f) { return f->isFile(); });
	copy_if(dirContent.begin(), dirContent.end(), back_inserter(files), [](const shared_ptr<FsNode> f) { return f->isDirectory(); });

	vector<string> filesWithAkaiName;
	transform(files.begin(), files.end(), back_inserter(filesWithAkaiName), [](const shared_ptr<FsNode> f) { return f->getName(); });
	remove_if(filesWithAkaiName.begin(), filesWithAkaiName.end(), [](const string& s) { return !AkaiName::isAkaiName(s); });

	vector<string> directoriesWithAkaiName;
	transform(directories.begin(), directories.end(), back_inserter(directoriesWithAkaiName), [](const shared_ptr<FsNode> dir) { return dir->getName(); });
	// Should check against ShortNameGenerator!!
	remove_if(directoriesWithAkaiName.begin(), directoriesWithAkaiName.end(), [](const string& s) { return !AkaiName::isAkaiName(s); });

	vector<shared_ptr<FsNode>> filesWithNoAkaiName;
	copy_if(files.begin(), files.end(), back_inserter(filesWithNoAkaiName), [](const shared_ptr<FsNode> f) { return !AkaiName::isAkaiName(f->getName()); });

	vector<shared_ptr<FsNode>> directoriesWithNoAkaiName;
	// Should check against ShortNameGenerator!!
	copy_if(directories.begin(), directories.end(), back_inserter(directoriesWithNoAkaiName), [](const shared_ptr<FsNode> dir) { return !AkaiName::isAkaiName(dir->getName()); });

	vector<string> allAkaiNames = filesWithAkaiName;
	copy(directoriesWithAkaiName.begin(), directoriesWithAkaiName.end(), back_inserter(allAkaiNames));

	for (auto& file : filesWithNoAkaiName) {
		auto akaiName = AkaiName::generate(file->getName(), allAkaiNames);
		file->renameTo(akaiName);
		allAkaiNames.push_back(akaiName);
	}

	for (auto& dir : directoriesWithNoAkaiName) {
		// Should use ShortNameGenerator!!
		auto akaiName = AkaiName::generate(dir->getName(), allAkaiNames);
		dir->renameTo(akaiName);
		allAkaiNames.push_back(akaiName);
	}

	/*
		else {
			//auto shortNameGenerator = moduru::raw::fat::ShortNameGenerator(usedNames);
			//string sn = shortNameGenerator.generateShortName(f->getName())->asSimpleString();
			//if (sn.find(".") != string::npos) {
			//	sn = sn.substr(0, sn.find_last_of("."));
			//}
			//usedNames.emplace(StrUtil::toLower(sn));
			//if (sn.compare(f->getName()) != 0) {
			//	f->renameTo(sn);
			//}
		}
	}
	*/
}

void StdDisk::initFiles()
{
	files.clear();
	allFiles.clear();

	renameFilesToAkai();

	auto view = diskGui->getView();
	auto fileArray = getDir().lock()->listFiles();
	for (auto& f : fileArray) {
		MpcFile* mpcFile = new MpcFile(f);
		allFiles.push_back(mpcFile);
		if (view != 0) {
			string name = f->getName();
			if (f->isFile() && name.find(".") != string::npos && name.substr(name.length() - 3).compare(extensions[view]) == 0) {
				files.push_back(mpcFile);
			}
		}
		else {
			files.push_back(mpcFile);
		}
	}
	initParentFiles();
}

void StdDisk::initParentFiles()
{
	parentFiles.clear();
	if (path.size() == 0) return;

	auto temp = getParentDir().lock()->listFiles();
	for (auto& f : temp) {
		if (f->isDirectory()) {
			parentFiles.push_back(new MpcFile(f));
		}
	}
}

string StdDisk::getDirectoryName()
{
	if (path.size() == 0) return "ROOT";
	return path[(int)(path.size()) - 1].lock()->getName();
}

bool StdDisk::moveBack()
{
	if (path.size() <= 0)
		return false;

	int lastPathIndex = (int)(path.size()) - 1;

	path.erase(path.begin() + lastPathIndex);
	return true;
}

bool StdDisk::moveForward(string directoryName)
{
	bool success = false;
	for (auto& f : files) {
		if (StrUtil::eqIgnoreCase(StrUtil::trim(f->getName()), StrUtil::trim(directoryName))) {
			auto lFile = f->getFsNode().lock();
			if (lFile->isDirectory() && lFile->getPath().find("vMPC") != string::npos && lFile->getPath().find("Stores") != string::npos) {
				path.push_back(dynamic_pointer_cast<moduru::file::Directory>(f->getFsNode().lock()));
				success = true;
				break;
			}
		}
	}
	return success;
}

weak_ptr<moduru::file::Directory> StdDisk::getDir()
{
	if (path.size() == 0)
		return root;

	return path[(int) (path.size()) - 1];
}

weak_ptr<moduru::file::Directory> StdDisk::getParentDir()
{
	if (path.size() == 0)
		return weak_ptr<Directory>();

	if (path.size() == 1)
		return root;

	return path[(int)(path.size()) - 2];
}

bool StdDisk::deleteAllFiles(int dwGuiDelete)
{
	weak_ptr<moduru::file::Directory> parentDirectory;
	try {
		parentDirectory = getParentDir();
	}
	catch (const exception& e1) {
		return false;
	}
	if (!parentDirectory.lock()) return false;

	auto success = false;
	auto files = parentDirectory.lock()->listFiles();
	for (auto& f : files) {
		if (!f->isDirectory()) {
			if (dwGuiDelete == 0 || StrUtil::hasEnding(f->getName(), extensions[dwGuiDelete])) {
				success = f->del();
			}
		}
	}
	return success;
}

bool StdDisk::newFolder(string newDirName)
{
	auto f = moduru::file::Directory(getDir().lock()->getPath() + FileUtil::getSeparator() + StrUtil::toUpper(newDirName), getDir().lock().get());
	return f.create();
}

bool StdDisk::deleteDir(MpcFile* f)
{
    return deleteRecursive(f->getFsNode().lock().get());
}

bool StdDisk::deleteRecursive(moduru::file::FsNode* deleteMe)
{
	auto deletedSomething = false;
	auto deletedCurrentFile = false;
	if (deleteMe->isDirectory()) {
		for (auto& f : dynamic_cast<moduru::file::Directory*>(deleteMe)->listFiles())
			deleteRecursive(f.get());
	}
	deletedCurrentFile = deleteMe->del();
	if (deletedCurrentFile)
		deletedSomething = true;

	if (!deletedCurrentFile)
		return false;

	return deletedSomething;
}

MpcFile* StdDisk::newFile(string newFileName)
{
	moduru::file::File* f = nullptr;
	try {
		auto split = FileUtil::splitName(newFileName);
		split[0] = moduru::lang::StrUtil::trim(split[0]);
		newFileName = split[0] + "." + split[1];

		string newFilePath = getDir().lock()->getPath() + FileUtil::getSeparator() + StrUtil::toUpper(StrUtil::replaceAll(newFileName, ' ', "_"));
		f = new moduru::file::File(newFilePath, getDir().lock().get());
		auto success = f->create();
		if (success) {
			return new MpcFile(shared_ptr<FsNode>(f));
		}
		else {
			delete f;
		}
		return nullptr;
	}
	catch (exception e) {
		e.what();
		if (f != nullptr) {
			delete f;
		}
		return nullptr;
	}
	return nullptr;
}

string StdDisk::getAbsolutePath()
{
    return getDir().lock()->getPath();
}

int StdDisk::getPathDepth() {
	return path.size();
}

StdDisk::~StdDisk() {
}
