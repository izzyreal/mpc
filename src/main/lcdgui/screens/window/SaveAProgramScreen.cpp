#include "SaveAProgramScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveAProgramScreen::SaveAProgramScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "save-a-program", layerIndex)
{
}

void SaveAProgramScreen::open()
{
	findField("replace-same-sounds").lock()->setAlignment(Alignment::Centered);
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

	auto nameScreen = mpc.screens->get<NameScreen>("name");

	switch (i)
	{
	case 3:
		openScreen("save");
		break;
	case 4:
	{
		auto fileName = mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()) + ".PGM";
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(fileName))
		{
			nameScreen->setName(program.lock()->getName());
			openScreen("file-exists");
			break;
		}

		disk->writePgm(program.lock(), fileName);
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
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	findLabel("file").lock()->setText(nameScreen->getNameWithoutSpaces() + ".PGM");
}
