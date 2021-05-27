#include "SaveApsFileScreen.hpp"

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>

#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

using namespace moduru::lang;

using namespace std;

SaveApsFileScreen::SaveApsFileScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-aps-file", layerIndex)
{
}

void SaveApsFileScreen::open()
{
	findField("replace-same-sounds").lock()->setAlignment(Alignment::Centered); 
	displayFile();
	displayReplaceSameSounds();
	displaySave();
}

void SaveApsFileScreen::turnWheel(int i)
{
	init();
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");

	if (param.compare("file") == 0)
	{
		nameScreen->parameterName = "saveapsname";
		openScreen("name");
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
		openScreen("save");
		break;
	case 4:
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
		string apsFileName = mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()) + ".APS";
        
        auto disk = mpc.getDisk().lock();

        if (disk->checkExists(apsFileName))
        {
            mpc.getLayeredScreen().lock()->openScreen("file-exists");
            return;
        }
        
		apsSaver = make_unique<mpc::disk::ApsSaver>(mpc, apsFileName);
        
        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("Saving " + StrUtil::padRight(nameScreen->getNameWithoutSpaces(), " ", 16) + ".APS");
        popupScreen->returnToScreenAfterMilliSeconds("save", 200);
        openScreen("popup");
		break;
	}
	}
}

void SaveApsFileScreen::displayFile()
{
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	findField("file").lock()->setText(nameScreen->getNameWithoutSpaces());
}

void SaveApsFileScreen::displaySave()
{
	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
	findField("save").lock()->setText(apsSaveNames[saveAProgramScreen->save]);
}

void SaveApsFileScreen::displayReplaceSameSounds()
{
	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
	findField("replace-same-sounds").lock()->setText(string(saveAProgramScreen->replaceSameSounds ? "YES" : "NO"));
}
