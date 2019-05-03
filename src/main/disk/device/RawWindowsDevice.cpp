#include <disk/device/RawWindowsDevice.hpp>

//#include <de/waldheinz/fs/FileSystem.hpp>
//#include <de/waldheinz/fs/FileSystemFactory.hpp>
//#include <de/waldheinz/fs/FsDirectory.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectory.hpp>
//#include <de/waldheinz/fs/util/RawWindowsVolume.hpp>

using namespace mpc::disk::device;
using namespace std;

RawWindowsDevice::RawWindowsDevice(string driveLetter, mpc::disk::RawDisk* disk, int totalSpace)
{
//    this->driveLetter = driveLetter;
//    this->totalSpace = totalSpace;
//    auto readOnly = true;
//    rawVolume_ = new ::de::waldheinz::fs::util::RawWindowsVolume(driveLetter, readOnly, totalSpace);
//    try {
//        fs_ = ::de::waldheinz::fs::FileSystemFactory::createAkai(rawVolume_, readOnly);
//    }
//    catch (exception e) {
//        e.what();
//    }
}

/*
de::waldheinz::fs::util::RawWindowsVolume*& RawWindowsDevice::rawVolume()
{
    return rawVolume_;
}
de::waldheinz::fs::util::RawWindowsVolume* RawWindowsDevice::rawVolume_;

de::waldheinz::fs::FileSystem*& RawWindowsDevice::fs()
{
    return fs_;
}
de::waldheinz::fs::FileSystem* RawWindowsDevice::fs_;
*/


std::any RawWindowsDevice::getFileSystem()
{
    //return fs_;
	return nullptr;
}

std::any RawWindowsDevice::getRoot()
{
	//return fs_->getRoot();
	return nullptr;
}

void RawWindowsDevice::close()
{
	try {
		//fs_->close();
		//rawVolume_->close();
	}
	catch (exception e) {
		e.what();
	}
}

void RawWindowsDevice::flush()
{
	try {
		//fs_->flush();
		//fs_->close();
		//fs_ = ::de::waldheinz::fs::FileSystemFactory::createAkai(rawVolume_, false);
	}
	catch (exception e) {
		e.what();
	}
}

string RawWindowsDevice::getAbsolutePath()
{
	return driveLetter;
}

int RawWindowsDevice::getSize()
{
	//return rawVolume_->getSize();
	return 0;
}

string RawWindowsDevice::getVolumeName()
{
    return "<no vol>";
}
