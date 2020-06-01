#include "DeleteFolderControls.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <Logger.hpp>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::controls::disk::dialog;
using namespace std;

DeleteFolderControls::DeleteFolderControls() 
	: AbstractDiskControls()
{
}

void DeleteFolderControls::static_deleteFolder(void* this_p)
{
	static_cast<DeleteFolderControls*>(this_p)->deleteFolder();
}

void DeleteFolderControls::deleteFolder()
{
	disk.lock()->setBusy(true);
	//ls.lock()->createPopup("Delete:" + directoryGui->getSelectedFile()->getName(), 85);
	//if (disk.lock()->deleteDir(directoryGui->getSelectedFile())) {
		//MLOG("About to flush.");
		//disk.lock()->flush();
		//disk.lock()->initFiles();
	//}
	this_thread::sleep_for(chrono::milliseconds(1000));
	ls.lock()->removePopup();
	ls.lock()->openScreen("directory");
	disk.lock()->setBusy(false);
}

void DeleteFolderControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 4:
		if (deleteFolderThread.joinable()) deleteFolderThread.join();
		deleteFolderThread = thread(&DeleteFolderControls::static_deleteFolder, this);
		break;
	}
}

DeleteFolderControls::~DeleteFolderControls() {
	if (deleteFolderThread.joinable()) deleteFolderThread.join();
}
