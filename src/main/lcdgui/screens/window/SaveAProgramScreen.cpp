#include "SaveAProgramScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog/FileExistsScreen.hpp>
#include "disk/MpcFile.hpp"
#include "lcdgui/screens/SaveScreen.hpp"

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveAProgramScreen::SaveAProgramScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "save-a-program", layerIndex)
{
}

void SaveAProgramScreen::open()
{
    if (ls->getPreviousScreenName() == "save")
    {
        auto nameScreen = mpc.screens->get<NameScreen>();
        auto saveScreen = mpc.screens->get<SaveScreen>();
        nameScreen->setName(sampler->getProgram(saveScreen->getProgramIndex())->getName());
    }

    findField("replace-same-sounds")->setAlignment(Alignment::Centered);
	displayFile();
	displaySave();
	displayReplaceSameSounds();
}

void SaveAProgramScreen::turnWheel(int i)
{
	init();

	if (param == "save")
	{
		setSave(save + i);
	}
	else if (param == "replace-same-sounds")
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
		openScreen("save");
		break;
	case 4:
	{
        auto nameScreen = mpc.screens->get<NameScreen>();
		auto fileName = mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()) + ".PGM";
		auto disk = mpc.getDisk();

		if (disk->checkExists(fileName))
		{
            auto replaceAction = [this, disk, fileName]{
                auto success = disk->getFile(fileName)->del();

                if (success)
                {
                    disk->flush();
                    disk->initFiles();
                    disk->writePgm(program, fileName);
                }
            };

            const auto initializeNameScreen = [this]{
                auto nameScreen = mpc.screens->get<NameScreen>();
                auto enterAction = [this](std::string&){ openScreen(name); };
                nameScreen->initialize(nameScreen->getNameWithoutSpaces(), 16, enterAction, "save");
            };

            auto fileExistsScreen = mpc.screens->get<FileExistsScreen>();
            fileExistsScreen->initialize(replaceAction, initializeNameScreen, [this]{ openScreen("save"); });
            openScreen("file-exists");
            break;
		}

		disk->writePgm(program, fileName);
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
	findField("save")->setText(pgmSaveNames[save]);
}

void SaveAProgramScreen::displayReplaceSameSounds()
{
	findField("replace-same-sounds")->setText(std::string(replaceSameSounds ? "YES" : "NO"));
}

void SaveAProgramScreen::displayFile()
{
	auto nameScreen = mpc.screens->get<NameScreen>();
	findLabel("file")->setText(nameScreen->getNameWithoutSpaces() + ".PGM");
}
