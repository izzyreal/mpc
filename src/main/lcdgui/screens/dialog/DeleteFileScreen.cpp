#include "DeleteFileScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace std;

DeleteFileScreen::DeleteFileScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-file", layerIndex)
{
}

void DeleteFileScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 1:
		ls.lock()->openScreen("delete-all-files");
		break;
	case 4:
		auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(mpc.screens->getScreenComponent("directory"));
		ls.lock()->openScreen("popup");
		auto popupScreen = dynamic_pointer_cast<PopupScreen>(mpc.screens->getScreenComponent("popup"));
		popupScreen->setText("Delete:" + directoryScreen->getSelectedFile()->getName());
		
		if (deleteThread.joinable())
		{
			deleteThread.join();
		}
		
		deleteThread = thread(&DeleteFileScreen::static_delete, this);
		break;
	}
}

void DeleteFileScreen::static_delete(void * args)
{
	static_cast<DeleteFileScreen*>(args)->deleteFile();
}

void DeleteFileScreen::deleteFile()
{
	this_thread::sleep_for(chrono::milliseconds(1000));

	auto disk = mpc.getDisk().lock();

	if (disk->deleteSelectedFile())
	{
		disk->flush();
		disk->initFiles();

		auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
		
		loadScreen->setFileLoad(loadScreen->fileLoad - 1);
		auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(mpc.screens->getScreenComponent("directory"));
		directoryScreen->setYOffset1(directoryScreen->yOffset1 - 1);
	}

	ls.lock()->openScreen("directory");
}
