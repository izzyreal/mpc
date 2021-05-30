#include "SaveAllFileScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::file::all;
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
		auto fileNameWithExt = fileName + ".ALL";
		auto disk = mpc.getDisk().lock();

		if (disk->checkExists(fileNameWithExt))
		{
			openScreen("file-exists");
			return;
		}
		
		allParser = make_unique<AllParser>(mpc, fileNameWithExt);
		
        auto f = disk->newFile(fileNameWithExt);
		auto bytes = allParser->getBytes();
		f->setFileData(&bytes);
		disk->flush();
		disk->initFiles();
        
        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("         Saving ...");
        popupScreen->returnToScreenAfterMilliSeconds("save", 200);
        openScreen("popup");
		break;
	}
	}
}
