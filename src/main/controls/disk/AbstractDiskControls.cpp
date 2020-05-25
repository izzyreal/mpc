#include <controls/disk/AbstractDiskControls.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <ui/Uis.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>
#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc::controls::disk;
using namespace std;

AbstractDiskControls::AbstractDiskControls()
	: BaseControls()
{
	auto uis = Mpc::instance().getUis().lock();
	directoryGui = Mpc::instance().getUis().lock()->getDirectoryGui();
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	diskGui = uis->getDiskGui();
	diskWindowGui = uis->getDiskWindowGui();
	deviceGui = uis->getDeviceGui();
}

void AbstractDiskControls::init()
{
	super::init();
	disk = Mpc::instance().getDisk();
	auto lDisk = disk.lock();
	if (csn.compare("loadasequencefromall") != 0 && lDisk && lDisk->getFiles().size() > 0 && diskGui->getFileLoad() < lDisk->getFiles().size()) {
		selectedFile = diskGui->getSelectedFile();
	}
}

AbstractDiskControls::~AbstractDiskControls() {
}
