#include "SaveAllFileScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog/FileExistsScreen.hpp>

#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveAllFileScreen::SaveAllFileScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-all-file", layerIndex)
{
}

void SaveAllFileScreen::open()
{
    if (ls->isPreviousScreen<SaveScreen>())
    {
        fileName = "ALL_SEQ_SONG1";
    }

	displayFile();
}

void SaveAllFileScreen::displayFile()
{
	if (fileName.empty())
    {
        findField("file")->setText("");
        findLabel("file1")->setText("");
        return;
    }

	findField("file")->setText(fileName.substr(0, 1));
    findLabel("file1")->setText(StrUtil::padRight(fileName.substr(1), " ", 15) + ".ALL");
}

void SaveAllFileScreen::openNameScreen()
{
	init();

	if (param == "file")
	{
        const auto enterAction = [this](std::string& nameScreenName) {
            fileName = nameScreenName;
            mpc.getLayeredScreen()->openScreen<SaveAllFileScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        nameScreen->initialize(fileName, 16, enterAction, name);
        mpc.getLayeredScreen()->openScreen<NameScreen>();
	}
}

void SaveAllFileScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
        mpc.getLayeredScreen()->openScreen<SaveScreen>();
		break;
	case 4:
	{
		auto allFileName = fileName + ".ALL";
		auto disk = mpc.getDisk();

		if (disk->checkExists(allFileName))
		{
            auto replaceAction = [disk, allFileName]{
                auto success = disk->getFile(allFileName)->del();

                if (success)
                {
                    disk->flush();
                    disk->initFiles();
                    disk->writeAll(allFileName);
                }
            };

            const auto initializeNameScreen = [this]{
                auto nameScreen = mpc.screens->get<NameScreen>();
                auto enterAction = [this](std::string& nameScreenName){
                    fileName = nameScreenName;
                    mpc.getLayeredScreen()->openScreen<SaveAllFileScreen>();
                };
                nameScreen->initialize(fileName, 16, enterAction, "save");
            };

            auto fileExistsScreen = mpc.screens->get<FileExistsScreen>();
            fileExistsScreen->initialize(replaceAction, initializeNameScreen, [this]{ mpc.getLayeredScreen()->openScreen<SaveScreen>(); });
            mpc.getLayeredScreen()->openScreen<FileExistsScreen>();
			return;
		}
		
        disk->writeAll(allFileName);
        break;
	}
	}
}
