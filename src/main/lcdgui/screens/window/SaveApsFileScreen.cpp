#include "SaveApsFileScreen.hpp"

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>

#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;

using namespace moduru::lang;

SaveApsFileScreen::SaveApsFileScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-aps-file", layerIndex)
{
}

void SaveApsFileScreen::open()
{
	findField("replace-same-sounds")->setAlignment(Alignment::Centered);
	displayFile();
	displayReplaceSameSounds();
	displaySave();
}

void SaveApsFileScreen::turnWheel(int i)
{
	init();
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");

	if (param == "file")
	{
        const auto saveApsFileScreen = this;
        
        auto renamer = [saveApsFileScreen](std::string& newName) {
            saveApsFileScreen->fileName = newName;
        };

        nameScreen->setName(fileName);
        nameScreen->setRenamerAndScreenToReturnTo(renamer, "save-aps-file");
        openScreen("name");
	}
	else if (param == "save")
	{
		saveAProgramScreen->setSave(saveAProgramScreen->save + i);
		displaySave();
	}
	else if (param == "replace-same-sounds")
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
        std::string apsFileName = fileName + ".APS";
        
        auto disk = mpc.getDisk();

        if (disk->checkExists(apsFileName))
        {
            ls->openScreen("file-exists");
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
	findField("file")->setText(fileName);
}

void SaveApsFileScreen::displaySave()
{
	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
	findField("save")->setText(apsSaveNames[saveAProgramScreen->save]);
}

void SaveApsFileScreen::displayReplaceSameSounds()
{
	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
	findField("replace-same-sounds")->setText(std::string(saveAProgramScreen->replaceSameSounds ? "YES" : "NO"));
}
