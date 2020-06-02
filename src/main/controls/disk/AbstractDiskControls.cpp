#include <controls/disk/AbstractDiskControls.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <ui/Uis.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>

#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc::controls::disk;
using namespace std;

AbstractDiskControls::AbstractDiskControls()
	: BaseControls()
{
	auto uis = Mpc::instance().getUis().lock();
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	diskGui = uis->getDiskGui();
	deviceGui = uis->getDeviceGui();
}

void AbstractDiskControls::init()
{
	super::init();
	disk = Mpc::instance().getDisk();
	auto lDisk = disk.lock();
	/*
	if (currentScreenName.compare("load-a-sequence-from-all") != 0 && lDisk && lDisk->getFiles().size() > 0 && diskGui->getFileLoad() < lDisk->getFiles().size()) {
		selectedFile = diskGui->getSelectedFile();
	}
	*/
}

AbstractDiskControls::~AbstractDiskControls() {
}
