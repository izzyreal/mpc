#include <disk/RawDisk.hpp>

#include <Mpc.hpp>

#include <disk/MpcFile.hpp>
#include <disk/Store.hpp>
#include <disk/device/Device.hpp>
#include <disk/device/RawOsxDevice.hpp>
#include <disk/device/RawWindowsDevice.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
//#include <de/waldheinz/fs/FsDirectory.hpp>
//#include <de/waldheinz/fs/FsDirectoryEntry.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectory.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectoryEntry.hpp>
//#include <de/waldheinz/fs/fat/FatFile.hpp>

using namespace mpc::disk;
using namespace std;

RawDisk::RawDisk(weak_ptr<Store> store, mpc::Mpc* mpc)
	: AbstractDisk(store, mpc)
{
	auto lStore = store.lock();
	try {
		//if (mpc::maingui::StartUp::osx()) device = new ::device::RawOsxDevice(store->path);

		//if (!mpc::maingui::StartUp::osx())
		device = make_unique<mpc::disk::device::RawWindowsDevice>("\\\\.\\" + lStore->path, this, lStore->totalSpace);

	}
	catch (exception e) {
		throw e;
	}
	//if (device != nullptr && device->getFileSystem() != nullptr) {
	if (device) {
		//path.clear();
		//root = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(device->getRoot());
		initFiles();
	}
}

void RawDisk::initFiles()
{
    //root = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(device->getRoot());
    refreshPath();
	files.clear();
    auto view = 0;

	//if (lGui) {
        //auto diskGui = lGui->getDiskGui();
        //view = diskGui->getView();
    //}
    /*
	Iterator* iterator = nullptr;
    if(path->size() == 0) {
        iterator = root->iterator();
    } else {
        try {
            iterator = npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(path->get(path->size() - 1)))->getDirectory()))->iterator();
        } catch (exception e) {
            e->printStackTrace();
        }
    }
    while (iterator->hasNext()) {
        auto entry = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(dynamic_cast< ::de::waldheinz::fs::FsDirectoryEntry* >(iterator->next()));
        if(entry->getName()->startsWith(".") || entry->getName()->length() == 0)
            continue;

        if(view != 0) {
            if(entry->isFile() && !npc(entry->getName()->toUpperCase())->endsWith((*extensions)[view]))
                continue;

        }
        files.push_back(new MpcFile(entry)));
    }
	*/
    initParentFiles();
}

void RawDisk::initParentFiles()
{
	parentFiles.clear();
//	if (path->size() == 0) return;

	//auto iterator = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(path[path->size() - 1]->getParent()->iterator();
	//while (iterator->hasNext()) {
		//auto entry = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(dynamic_cast< ::de::waldheinz::fs::FsDirectoryEntry* >(iterator->next()));
		//if(entry->getName()->startsWith(".") || entry->getName()->length() == 0)
		 //   continue;

	  //  if(entry->isValid() && entry->isDirectory())
		//    parentFiles.push_back(new MpcFile(entry)));

	//}
}

string RawDisk::getDirectoryName()
{
    //if(path.size() == 0) return "ROOT";

    //return dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(path[(int)(path->size()) -1])->getName();
	return "";
}

bool RawDisk::moveBack()
{
	//if (path.size() == 0) return false;

//	path->remove(path->size() - 1);
	refreshPath();
//	return true;
	return false;
}

bool RawDisk::moveForward(string directoryName)
{
	/*
	::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry = nullptr;
	try {
		entry = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(npc(getDir())->getEntry(directoryName));
		if(entry == nullptr || entry->isFile())
			return false;

	} catch (exception e) {
		e.what();
		return false;
	}
	path.push_back(entry);
	refreshPath();
	return true;
	*/
	return false;
}

int RawDisk::getPathDepth() {
	//return path.size() == 0;
	return true;
}

/*
de::waldheinz::fs::fat::AkaiFatLfnDirectory* RawDisk::getDir() 
{
    if(path->size() == 0)
        return root;

    return dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(path->get(path->size() - 1)))->getDirectory());
}

de::waldheinz::fs::fat::AkaiFatLfnDirectory* RawDisk::getParentDir() 
{
    if(path->size() == 0)
        return root;

    if(path->size() == 1)
        return root;

    return dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(path->get(path->size() - 2)))->getDirectory());
}
*/

void RawDisk::refreshPath()
{
    /*
	root = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(device->getRoot());
    if(path->size() > 0) {
        ::java::util::List* refreshedPath = new ::java::util::ArrayList();
        auto directory = root;
        for (auto _i = path->iterator(); _i->hasNext(); ) {
            ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* e = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(_i->next());
            {
                npc(refreshedPath).push_back(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(npc(directory)->getEntry(e->getName()))));
                try {
                    directory = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(npc(directory)->getEntry(e->getName())))->getDirectory());
                } catch (exception e1) {
                    npc(e1)->printStackTrace();
                }
            }
        }
        path = refreshedPath;
    }
	*/
}

/*
File* RawDisk::entryToFile(::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry)
{

	::java::io::FileOutputStream* fos;
	::java::nio::ByteBuffer* buffer;
	auto pathString = mpc::maingui::StartUp::tempPath();
	try {
		fos = new ::java::io::FileOutputStream((pathString)->append("/")
			->append(entry->getName())->toString());
		auto length = static_cast<int32_t>(npc(entry->getFile())->getLength());
		auto bufferLength = length;
		auto clusterSize = 2048;
		if (length > clusterSize) {
			bufferLength = clusterSize;
			auto numberOfWholeBuffers = static_cast<int32_t>(::java::lang::Math::floor(length / clusterSize));
			for (auto j = 0; j < numberOfWholeBuffers; j++) {
				buffer = ::java::nio::ByteBuffer::allocateDirect(bufferLength);
				npc(entry->getFile())->read(static_cast<int64_t>((j * clusterSize)), buffer);
				auto data = new ::int8_tArray(bufferLength);
				npc(buffer)->position(0);
				for (int i = 0; i < npc(data)->length; i++)
					(*data)[i] = npc(buffer)->get();

				fos->write(data);
			}
			auto remaining = length - (numberOfWholeBuffers * clusterSize);
			buffer = ::java::nio::ByteBuffer::allocateDirect(remaining);
			npc(entry->getFile())->read(static_cast<int64_t>(numberOfWholeBuffers * clusterSize), buffer);
			auto data = new ::int8_tArray(bufferLength);
			npc(buffer)->position(0);
			for (int i = 0; i < remaining; i++)
				(*data)[i] = npc(buffer)->get();

			fos->write(data);
		}
		else {
			buffer = ::java::nio::ByteBuffer::allocateDirect(bufferLength);
			npc(entry->getFile())->read(static_cast<int64_t>(0), buffer);
			auto data = new ::int8_tArray(bufferLength);
			npc(buffer)->position(0);
			for (int i = 0; i < npc(data)->length; i++)
				(*data)[i] = npc(buffer)->get();

			fos->write(data);
		}
		fos->close();
	}
	catch (exception e) {
		e->printStackTrace();
		return nullptr;
	}
	return new File(pathString + "/" + entry->getName());
}
*/

bool RawDisk::deleteDir(MpcFile* f)
{
	/*
	auto entry = f->getEntry();
	auto left = lGui->getDirectoryGui()->getXPos() == 0;
	try {
		deleteFilesRecursive(entry);
		return deleteDirsRecursive(entry, left);
	}
	catch (exception e) {
		e->printStackTrace();
		return false;
	}
	*/
	return false;
}

/*
bool RawDisk::deleteFilesRecursive(::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry) 
{
    if(entry->getName()->startsWith(".") || entry->getName()->length() == 0)
        return false;

    auto deletedSomething = false;
    auto deletedCurrentFile = false;
    if(entry->isDirectory()) {
        auto iterator = npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(entry->getDirectory()))->iterator();
        ::java::util::List* toRemove = new ::java::util::ArrayList();
        while (iterator->hasNext()) 
                        npc(toRemove).push_back(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(dynamic_cast< ::de::waldheinz::fs::FsDirectoryEntry* >(iterator->next()))));

        for (auto _i = npc(toRemove)->iterator(); _i->hasNext(); ) {
            ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* subEntry = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(_i->next());
            
                                deleteFilesRecursive(subEntry);

        }
    }
    if(entry->isFile()) {
        try {
            entry->getParent()->remove(entry->getName());
            deletedCurrentFile = true;
        } catch (exception e) {
            deletedCurrentFile = false;
        }
    }
    if(deletedCurrentFile)
        deletedSomething = true;

    return deletedSomething;
}

bool RawDisk::deleteDirsRecursive(::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* entry, bool checkExist) 
{
    if(entry->getName()->startsWith(".") || entry->getName()->length() == 0)
        return false;

    auto dirGui = lGui->getDirectoryGui();
    auto deletedSomething = false;
    auto deletedCurrentDir = false;
    if(entry->isFile())
        return false;

    ::java::util::List* list = new ::java::util::ArrayList();
    auto iterator = npc(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(entry->getDirectory()))->iterator();
    while (iterator->hasNext()) 
                list.push_back(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(dynamic_cast< ::de::waldheinz::fs::FsDirectoryEntry* >(iterator->next()))));

    for (auto _i = list->iterator(); _i->hasNext(); ) {
        ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* fe = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(_i->next());
        {
            if(npc(fe)->isDirectory())
                deleteDirsRecursive(fe, false);

        }
    }
    try {
        npc(entry->getParent())->remove(entry->getName());
        deletedCurrentDir = true;
    } catch (exception e) {
        deletedCurrentDir = false;
        e->printStackTrace();
    }
    if(deletedCurrentDir)
        deletedSomething = true;

    if(!deletedCurrentDir)
        throw new ::java::io::FileNotFoundException();

    if(checkExist) {
        auto deletedDirNumber = dirGui->getYOffsetFirst() + dirGui->getYpos0();
        auto newDirNumber = deletedDirNumber;
        auto parent = getParentDir();
        parentFiles->remove(deletedDirNumber);
        if(dirGui->getYOffsetFirst() + dirGui->getYpos0() > parentFiles->size() - 1) {
            if(dirGui->getYOffsetFirst() != 0) {
                dirGui->setYOffset0(dirGui->getYOffsetFirst() - 1);
            } else {
                if(dirGui->getYpos0() > 0)
                    dirGui->setYPos0(dirGui->getYpos0() - 1);

            }
        }
        if(newDirNumber > parentFiles->size() - 1)
            newDirNumber--;

        if(parentFiles->size() == 0) {
            path->remove(path->size() - 1);
            refreshPath();
        } else {
            path->remove(path->size() - 1);
            path.push_back(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(npc(parent)->getEntry(npc(dynamic_cast< MpcFile* >(parentFiles->get(newDirNumber)))->getName()))));
            refreshPath();
        }
    }
    return deletedSomething;
}
*/

bool RawDisk::deleteAllFiles(int dwGuiDelete)
{
	/*
	::de::waldheinz::fs::fat::AkaiFatLfnDirectory* parentDirectory = nullptr;
	try {
		parentDirectory = getParentDir();
	} catch (exception e1) {
		return false;
	}
	npc(::java::lang::System::out())->println(("parent directory ")->append(static_cast< ::java::lang::Object* >(parentDirectory))->toString());
	auto success = false;
	auto iterator = npc(parentDirectory)->iterator();
	::java::util::List* list = new ::java::util::ArrayList();
	while (iterator->hasNext())
				list.push_back(dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(dynamic_cast< ::de::waldheinz::fs::FsDirectoryEntry* >(iterator->next()))));

	for (auto _i = list->iterator(); _i->hasNext(); ) {
		::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* f = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(_i->next());
		{
			if(!f->isDirectory()) {
				if(dwGuiDelete == 0 || npc(f->getName()->toUpperCase())->endsWith((*extensions)[dwGuiDelete])) {
					try {
						npc(f->getParent())->remove(f->getName());
					} catch (exception e) {
						success = false;
					}
					success = true;
				}
			}
		}
	}
	return success;
	*/
	return false;
}

bool RawDisk::newFolder(string newDirName)
{
	try {
		//dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(npc(getDir())->addDirectory(newDirName));
	}
	catch (exception e) {
		return false;
	}
	return true;
}

MpcFile* RawDisk::newFile(string newFileName)
{
	/*
    try {
        auto newEntry = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(npc(getDir())->addFile(newFileName->replaceAll(" ", "_")));
        return new MpcFile(newEntry);
    } catch (exception e) {
        //println("IOException. Prolly filename already exists.");
        return nullptr;
    }
	*/
	return nullptr;
}

string RawDisk::getAbsolutePath()
{
	/*
	auto pathString = "";
	for (auto& a : path) {
		pathString = pathString + "/" + a->getName();
	}
	return device->getAbsolutePath() + pathString;
	*/
	return device->getAbsolutePath();
}
