#include "DeleteAllFilesScreen.hpp"

#include <disk/AbstractDisk.hpp>

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>

using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace std;

DeleteAllFilesScreen::DeleteAllFilesScreen(const int layerIndex)
	: ScreenComponent("delete-all-files", layerIndex)
{
}

void DeleteAllFilesScreen::open()
{
	displayDelete();
}

void DeleteAllFilesScreen::displayDelete()
{
	findField("delete").lock()->setText(views[delete_]);
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
	init();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("deletefile");
		ls.lock()->setPreviousScreenName("directory");
		break;
	case 4:
	{
		auto success = mpc.getDisk().lock()->deleteAllFiles(delete_);

		if (success)
		{
			auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
			auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(Screens::getScreenComponent("directory"));
			loadScreen->fileLoad = 0;
			directoryScreen->yOffset1 = 0;
			mpc.getDisk().lock()->initFiles();
		}
		ls.lock()->openScreen("directory");
		break;
	}
	}
}
