#include <controls/disk/AbstractDiskControls.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <ui/Uis.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc::controls::disk;
using namespace std;

AbstractDiskControls::AbstractDiskControls(mpc::Mpc* mpc)
	: AbstractControls(mpc)
{
	auto uis = mpc->getUis().lock();
	directoryGui = mpc->getUis().lock()->getDirectoryGui();
	nameGui = mpc->getUis().lock()->getNameGui();
	diskGui = uis->getDiskGui();
	diskWindowGui = uis->getDiskWindowGui();
	deviceGui = uis->getDeviceGui();
	soundGui = uis->getSoundGui();
}

void AbstractDiskControls::init()
{
	super::init();
	disk = mpc->getDisk();
	auto lDisk = disk.lock();
	if (csn.compare("loadasequencefromall") != 0 && lDisk && lDisk->getFiles().size() > 0 && diskGui->getFileLoad() < lDisk->getFiles().size()) {
		selectedFile = diskGui->getSelectedFile();
	}
}

AbstractDiskControls::~AbstractDiskControls() {
}
