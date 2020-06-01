#include "SaveApsFileScreen.hpp"

#include <Util.hpp>
#include <ui/NameGui.hpp>

#include <lcdgui/screens/window/SaveAProgramScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveApsFileScreen::SaveApsFileScreen(const int layerIndex)
	: ScreenComponent("save-aps-file", layerIndex)
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

	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(Screens::getScreenComponent("save-a-program"));

	if (param.compare("file") == 0)
	{
		nameGui->setParameterName("saveapsname");
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
		string apsFileName = mpc::Util::getFileName(nameGui->getName()) + ".APS";
		apsSaver = make_unique<mpc::disk::ApsSaver>(apsFileName);
		break;
	}
	}
}

void SaveApsFileScreen::displayFile()
{
	auto nameGui = mpc.getUis().lock()->getNameGui();
	findField("file").lock()->setText(nameGui->getName());
}

void SaveApsFileScreen::displaySave()
{
	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(Screens::getScreenComponent("save-a-program"));
	findField("save").lock()->setText(apsSaveNames[saveAProgramScreen->save]);
}

void SaveApsFileScreen::displayReplaceSameSounds()
{
	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(Screens::getScreenComponent("save-a-program"));
	findField("replace-same-sounds").lock()->setText(string(saveAProgramScreen->replaceSameSounds ? "YES" : "NO"));
}
