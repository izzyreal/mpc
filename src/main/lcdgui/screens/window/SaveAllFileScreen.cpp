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
	if (fileName.length() == 0)
    {
        findField("file").lock()->setText("");
        findLabel("file1").lock()->setText("");
        return;
    }

	findField("file").lock()->setText(fileName.substr(0, 1));
    findLabel("file1").lock()->setText(StrUtil::padRight(fileName.substr(1), " ", 15) + ".ALL");
}

void SaveAllFileScreen::turnWheel(int i)
{
	init();

	if (param.compare("file") == 0)
	{
        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto saveAllFileScreen = this;
        
        auto renamer = [saveAllFileScreen](string& newName) {
            saveAllFileScreen->fileName = newName;
        };

        nameScreen->setName(fileName);
        nameScreen->setRenamerAndScreenToReturnTo(renamer, "save-all-file");
        openScreen("name");
	}
}

void SaveAllFileScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		openScreen("save");
		break;
	case 4:
	{
		auto allFileName = fileName + ".ALL";
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(allFileName))
		{
			openScreen("file-exists");
			return;
		}
		
        disk->writeAll(allFileName);
        break;
	}
	}
}
