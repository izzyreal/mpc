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
	auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));

	if (nameScreen->getName().length() < 2)
	{
		return;
	}

	findField("file").lock()->setText(nameScreen->getName().substr(0, 1));
	findLabel("file1").lock()->setText(StrUtil::padRight(nameScreen->getName().substr(1), " ", 15) + ".ALL");
}

void SaveAllFileScreen::turnWheel(int i)
{
	init();

	auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));

	if (param.compare("file") == 0)
	{
		nameScreen->parameterName = "save-all-file";
		ls.lock()->openScreen("name");
	}
}

void SaveAllFileScreen::function(int i)
{
	init();

	auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
	{
		auto allName = mpc::Util::getFileName(nameScreen->getName());
		auto existStr = allName + ".ALL";
		
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(existStr))
		{
			ls.lock()->openScreen("file-already-exists");
			return;
		}
		
		allParser = make_unique<mpc::file::all::AllParser>(mpc, mpc::Util::getFileName(nameScreen->getName()));
		auto f = disk->newFile(allName + ".ALL");
		auto bytes = allParser->getBytes();
		f->setFileData(&bytes);
		disk->flush();
		disk->initFiles();
		ls.lock()->openScreen("save");
		break;
	}
	}
}
