#include <controls/disk/dialog/DeleteFileControls.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <ui/disk/DiskGui.hpp>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::controls::disk::dialog;
using namespace std;

DeleteFileControls::DeleteFileControls()
	: AbstractDiskControls()
{
}

void DeleteFileControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 1:
		ls.lock()->openScreen("delete-all-files");
		break;
	case 4:
		//ls.lock()->createPopup("Delete:" + directoryGui->getSelectedFile()->getName(), 85);
		if (deleteThread.joinable()) deleteThread.join();
		deleteThread = thread(&DeleteFileControls::static_delete, this);
		break;
	}
}

void DeleteFileControls::static_delete(void * args)
{
	static_cast<DeleteFileControls*>(args)->deleteFile();
}

void DeleteFileControls::deleteFile() {
	this_thread::sleep_for(chrono::milliseconds(1000));
	ls.lock()->removePopup();
	auto lDisk = disk.lock();
	if (lDisk->deleteSelectedFile()) {
		lDisk->flush();
		lDisk->initFiles();
		//diskGui->setFileLoad(diskGui->getFileLoad() - 1);
		//directoryGui->setYOffset1(directoryGui->getYOffsetSecond() - 1);
	}
	//if (diskGui->getFileLoad() < 0) diskGui->setFileLoad(0);

	ls.lock()->openScreen("directory");
}
