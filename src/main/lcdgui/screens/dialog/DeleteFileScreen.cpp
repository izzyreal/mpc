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
		openScreen("delete-all-files");
		break;
	case 4:
		auto directoryScreen = mpc.screens->get<DirectoryScreen>();
		openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>();
		popupScreen->setText("Delete:" + directoryScreen->getSelectedFile()->getName());
		
		if (deleteThread.joinable())
			deleteThread.join();
		
		deleteThread = std::thread(&DeleteFileScreen::static_delete, this);
		break;
	}
}

void DeleteFileScreen::static_delete(void * args)
{
	static_cast<DeleteFileScreen*>(args)->deleteFile();
}

void DeleteFileScreen::deleteFile()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(400));

	auto disk = mpc.getDisk();

	if (disk->deleteSelectedFile())
	{
		disk->flush();
		disk->initFiles();

		auto loadScreen = mpc.screens->get<LoadScreen>();
		
		loadScreen->setFileLoad(loadScreen->fileLoad - 1);
		auto directoryScreen = mpc.screens->get<DirectoryScreen>();
		directoryScreen->setYOffset1(directoryScreen->yOffset1 - 1);
	}

	openScreen("directory");
}

DeleteFileScreen::~DeleteFileScreen()
{
	if (deleteThread.joinable())
		deleteThread.join();
}
