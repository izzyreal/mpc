#include "SaveAllFileScreen.hpp"

#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>
#include <ui/NameGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveAllFileScreen::SaveAllFileScreen(const int layerIndex)
	: ScreenComponent("save-all-file", layerIndex)
{
}

void SaveAllFileScreen::open()
{
	displayFile();
}

void SaveAllFileScreen::displayFile()
{
	findField("file").lock()->setText(mpc.getUis().lock()->getNameGui()->getName().substr(0, 1));
	findLabel("file1").lock()->setText(moduru::lang::StrUtil::padRight(mpc.getUis().lock()->getNameGui()->getName().substr(1), " ", 15) + ".ALL");
}

void SaveAllFileScreen::turnWheel(int i)
{
	init();

	if (param.compare("file") == 0)
	{
		nameGui->setParameterName("save-all-file");
		ls.lock()->openScreen("name");
	}
}

void SaveAllFileScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
	{
		auto allName = mpc::Util::getFileName(nameGui->getName());
		auto existStr = allName + ".ALL";
		
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(existStr))
		{
			ls.lock()->openScreen("filealreadyexists");
			return;
		}
		
		allParser = make_unique<mpc::file::all::AllParser>(mpc::Util::getFileName(nameGui->getName()));
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
