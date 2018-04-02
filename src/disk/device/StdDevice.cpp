#include <disk/device/StdDevice.hpp>

#include <file/Directory.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::disk::device;
using namespace moduru::file;
using namespace std;

StdDevice::StdDevice(string rootPath)
{
	if (!FileUtil::Exists(rootPath)) {
		return;
	}
	root = make_shared<moduru::file::Directory>(rootPath, nullptr);
	valid = true;
}

boost::any StdDevice::getRoot()
{
	return weak_ptr<moduru::file::Directory>(root);
}

bool StdDevice::isValid()
{
    return valid;
}

void StdDevice::close()
{
}

void StdDevice::flush()
{
}

boost::any StdDevice::getFileSystem()
{
    return nullptr;
}

string StdDevice::getAbsolutePath()
{
    return "stddevabspath";
}

int StdDevice::getSize()
{
    return 909303;
}

string StdDevice::getVolumeName()
{
    return "stddevvolume";
}

StdDevice::~StdDevice() {
}
