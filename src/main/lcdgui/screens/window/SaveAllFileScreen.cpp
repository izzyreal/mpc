#include "SaveAllFileScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

SaveAllFileScreen::SaveAllFileScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-all-file", layerIndex)
{
}

void SaveAllFileScreen::open()
{
	displayFile();
}

void SaveAllFileScreen::displayFile()
{
	auto nameScreen = mpc.screens->get<NameScreen>("name");

	if (nameScreen->getNameWithoutSpaces().length() < 2)
		return;

	findField("file").lock()->setText(nameScreen->getNameWithoutSpaces().substr(0, 1));
	findLabel("file1").lock()->setText(StrUtil::padRight(nameScreen->getNameWithoutSpaces().substr(1), " ", 15) + ".ALL");
}

void SaveAllFileScreen::turnWheel(int i)
{
	init();

	auto nameScreen = mpc.screens->get<NameScreen>("name");

	if (param.compare("file") == 0)
	{
		nameScreen->parameterName = "save-all-file";
		openScreen("name");
	}
}

void SaveAllFileScreen::function(int i)
{
	init();

	auto nameScreen = mpc.screens->get<NameScreen>("name");

	switch (i)
	{
	case 3:
		openScreen("save");
		break;
	case 4:
	{
		auto allName = mpc::Util::getFileName(nameScreen->getNameWithoutSpaces());
		auto existStr = allName + ".ALL";
		
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(existStr))
		{
			openScreen("file-exists");
			return;
		}
		
		allParser = make_unique<mpc::file::all::AllParser>(mpc, mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()));
		auto f = disk->newFile(allName + ".ALL");
		auto bytes = allParser->getBytes();
		f->setFileData(&bytes);
		disk->flush();
		disk->initFiles();
		openScreen("save");
		break;
	}
	}
}
