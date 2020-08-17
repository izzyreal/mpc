#include "DeleteFolderScreen.hpp"

#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace std;

DeleteFolderScreen::DeleteFolderScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "delete-folder", layerIndex)
{
}

void DeleteFolderScreen::static_deleteFolder(void* this_p)
{
	static_cast<DeleteFolderScreen*>(this_p)->deleteFolder();
}

void DeleteFolderScreen::deleteFolder()
{
	auto disk = mpc.getDisk().lock();
	disk->setBusy(true);

	auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(mpc.screens->getScreenComponent("directory"));
	ls.lock()->openScreen("popup");
	auto popupScreen = dynamic_pointer_cast<PopupScreen>(mpc.screens->getScreenComponent("popup"));
	popupScreen->setText("Delete:" + directoryScreen->getSelectedFile()->getName());

	if (disk->deleteDir(directoryScreen->getSelectedFile()))
	{
		disk->flush();
		disk->initFiles();
	}

	this_thread::sleep_for(chrono::milliseconds(1000));
	ls.lock()->openScreen("directory");
	disk->setBusy(false);
}

void DeleteFolderScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 4:
		if (deleteFolderThread.joinable())
		{
			deleteFolderThread.join();
		}
		deleteFolderThread = thread(&DeleteFolderScreen::static_deleteFolder, this);
		break;
	}
}

DeleteFolderScreen::~DeleteFolderScreen() {
	if (deleteFolderThread.joinable())
	{
		deleteFolderThread.join();
	}
}
