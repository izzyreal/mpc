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
        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto saveApsFileScreen = this;
        
        auto renamer = [saveApsFileScreen](string& newName) {
            saveApsFileScreen->fileName = newName;
        };

        nameScreen->setName(fileName);
        nameScreen->setRenamerAndScreenToReturnTo(renamer, "save-aps-file");
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
		string apsFileName = fileName + ".APS";
        
        auto disk = mpc.getDisk().lock();

        if (disk->checkExists(apsFileName))
        {
            mpc.getLayeredScreen().lock()->openScreen("file-exists");
            return;
        }
        
		disk->writeAps(apsFileName);
        break;
	}
	}
}

void SaveApsFileScreen::displayFile()
{
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	findField("file").lock()->setText(fileName);
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
