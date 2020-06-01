#include "SaveAProgramScreen.hpp"

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveAProgramScreen::SaveAProgramScreen(const int layerIndex) 
	: ScreenComponent("save-a-program", layerIndex)
{
}

void SaveAProgramScreen::open()
{
	displayFile();
	displaySave();
	displayReplaceSameSounds();
}

void SaveAProgramScreen::turnWheel(int i)
{
	init();

	if (param.compare("save") == 0)
	{
		setSave(save + i);
	}
	else if (param.compare("replace-same-sounds") == 0)
	{
		replaceSameSounds = i > 0;
		displayReplaceSameSounds();
	}
}

void SaveAProgramScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
	{
		auto fileName = mpc::Util::getFileName(nameGui->getName()) + ".PGM";
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(fileName))
		{
			nameGui->setName(program.lock()->getName());
			ls.lock()->openScreen("filealreadyexists");
			break;
		}

		disk->writeProgram(program.lock().get(), fileName);
		break;
	}
	}
}

void SaveAProgramScreen::setSave(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	save = i;
	displaySave();
}

void SaveAProgramScreen::displaySave()
{
	findField("save").lock()->setText(pgmSaveNames[save]);
}

void SaveAProgramScreen::displayReplaceSameSounds()
{
	findField("replace-same-sounds").lock()->setText(string(replaceSameSounds ? "YES" : "NO"));
}

void SaveAProgramScreen::displayFile()
{
	auto nameGui = Mpc::instance().getUis().lock()->getNameGui();
	findLabel("file").lock()->setText(nameGui->getName() + ".PGM");
}
