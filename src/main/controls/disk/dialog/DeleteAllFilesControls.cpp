#include <controls/disk/dialog/DeleteAllFilesControls.hpp>

#include <disk/AbstractDisk.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>

using namespace mpc::controls::disk::dialog;
using namespace std;

DeleteAllFilesControls::DeleteAllFilesControls() 
	: AbstractDiskControls()
{
}

void DeleteAllFilesControls::turnWheel(int i) {
	init();
	diskWindowGui->setDelete(diskWindowGui->getDelete() + i);
}

void DeleteAllFilesControls::function(int i)
{
	init();
	bool success = false;
	switch (i) {
	case 3:
		ls.lock()->openScreen("deletefile");
		ls.lock()->setPreviousScreenName("directory");
		break;
	case 4:
		success = disk.lock()->deleteAllFiles(diskWindowGui->getDelete());

		if (success)
		{
			//diskGui->setFileLoad(0);
			//directoryGui->setYOffset1(0);
			disk.lock()->initFiles();
		}
		ls.lock()->openScreen("directory");
		break;
	}
}
