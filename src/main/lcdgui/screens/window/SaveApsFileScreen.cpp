#include "SaveApsFileScreen.hpp"

#include <Util.hpp>

#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveApsFileScreen::SaveApsFileScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-aps-file", layerIndex)
{
}

void SaveApsFileScreen::open()
{
	displayFile();
	displayReplaceSameSounds();
	displaySave();
}

void SaveApsFileScreen::turnWheel(int i)
{
	init();
	auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));
	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(mpc.screens->getScreenComponent("save-a-program"));

	if (param.compare("file") == 0)
	{
		nameScreen->parameterName = "saveapsname";
		ls.lock()->openScreen("name");
	}
	else if (param.compare("save") == 0)
	{
		saveAProgramScreen->setSave(saveAProgramScreen->save + i);
		displaySave();
	}
	else if (param.compare("replace-same-sounds") == 0)
	{
		saveAProgramScreen->replaceSameSounds = i > 0;
		displayReplaceSameSounds();
	}
}

void SaveApsFileScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
	{
		auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));
		string apsFileName = mpc::Util::getFileName(nameScreen->getName()) + ".APS";
		apsSaver = make_unique<mpc::disk::ApsSaver>(mpc, apsFileName);
		break;
	}
	}
}

void SaveApsFileScreen::displayFile()
{
	auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));
	findField("file").lock()->setText(nameScreen->getName());
}

void SaveApsFileScreen::displaySave()
{
	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(mpc.screens->getScreenComponent("save-a-program"));
	findField("save").lock()->setText(apsSaveNames[saveAProgramScreen->save]);
}

void SaveApsFileScreen::displayReplaceSameSounds()
{
	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(mpc.screens->getScreenComponent("save-a-program"));
	findField("replace-same-sounds").lock()->setText(string(saveAProgramScreen->replaceSameSounds ? "YES" : "NO"));
}
