#include <disk/device/RawOsxDevice.hpp>

//#include <de/waldheinz/fs/FileSystem.hpp>
//#include <de/waldheinz/fs/FileSystemFactory.hpp>
//#include <de/waldheinz/fs/FsDirectory.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectory.hpp>
//#include <de/waldheinz/fs/util/RawOsxVolume.hpp>

using namespace mpc::disk::device;
using namespace std;

RawOsxDevice::RawOsxDevice(string fileName)
{
//    this->absolutePath = fileName;
//    auto readOnly = true;
//    rawVolume_ = ::de::waldheinz::fs::util::RawOsxVolume(fileName, readOnly);
//    fs_ = ::de::waldheinz::fs::FileSystemFactory::createAkai(rawVolume_, readOnly);
}

/*
de::waldheinz::fs::util::RawOsxVolume*& RawOsxDevice::rawVolume()
{
    
    return rawVolume_;
}
de::waldheinz::fs::util::RawOsxVolume* RawOsxDevice::rawVolume_;

de::waldheinz::fs::FileSystem*& RawOsxDevice::fs()
{
    
    return fs_;
}
de::waldheinz::fs::FileSystem* RawOsxDevice::fs_;
*/

std::any RawOsxDevice::getRoot()
{
    try {
        //return java_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectory* >(fs_->getRoot());
    } catch (exception e) {
        e.what();
    }
    return nullptr;
}

std::any RawOsxDevice::getFileSystem()
{
	//return fs_;
	return nullptr;
}

void RawOsxDevice::close()
{
	try {
		//fs_->close();
		//rawVolume_->close();
	}
	catch (exception e) {
		e.what();
	}
}

void RawOsxDevice::flush()
{
	try {
		//fs_->flush();
		//fs_->close();
		//fs_ = ::de::waldheinz::fs::FileSystemFactory::createAkai(rawVolume_, false);
		//rawVolume_->flush();
	}
	catch (exception e) {
		e.what();
	}
}

string RawOsxDevice::getAbsolutePath()
{
    return absolutePath;
}

int RawOsxDevice::getSize()
{
    try {
        //return rawVolume_->getSize();
    } catch (exception e) {
        e.what();
        return 0;
    }
	return 0;
}

string RawOsxDevice::getVolumeName()
{
    try {
        //return fs_->getRoot();
    } catch (exception e) {
        e.what();
        return "<no vol>";
    }
	return "<no vol>";
}
