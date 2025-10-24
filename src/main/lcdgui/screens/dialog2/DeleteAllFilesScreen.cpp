#include "DeleteAllFilesScreen.hpp"

#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"

using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

DeleteAllFilesScreen::DeleteAllFilesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-files", layerIndex)
{
}

void DeleteAllFilesScreen::open()
{
	displayDelete();
}

void DeleteAllFilesScreen::displayDelete()
{
	findField("delete")->setText(views[delete_]);
}

void DeleteAllFilesScreen::setDelete(int i)
{
	if (i < 0 || i > 8)
	{
		return;
	}
	delete_ = i;
	displayDelete();
}

void DeleteAllFilesScreen::turnWheel(int i) {
	setDelete(delete_ + i);
}

void DeleteAllFilesScreen::function(int i)
{

	switch (i)
	{
	case 3:
        mpc.getLayeredScreen()->openScreen<DeleteFileScreen>();
		break;
	case 4:
	{
		auto success = mpc.getDisk()->deleteAllFiles(delete_);

		if (success)
		{
			auto loadScreen = mpc.screens->get<LoadScreen>();
			auto directoryScreen = mpc.screens->get<DirectoryScreen>();
			loadScreen->fileLoad = 0;
			directoryScreen->yOffset1 = 0;
			mpc.getDisk()->initFiles();
		}

        mpc.getLayeredScreen()->openScreen<DirectoryScreen>();
		break;
	}
	}
}
