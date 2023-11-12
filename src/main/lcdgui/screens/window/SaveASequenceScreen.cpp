#include "SaveASequenceScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog/FileExistsScreen.hpp>

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveASequenceScreen::SaveASequenceScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-a-sequence", layerIndex)
{
}

void SaveASequenceScreen::open()
{
    if (ls->getPreviousScreenName() == "save")
    {
        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        nameScreen->setName(sequencer.lock()->getActiveSequence()->getName());
    }

    displaySaveAs();
	displayFile();
}

void SaveASequenceScreen::turnWheel(int i)
{
	init();

	if (param == "saveas")
	{
		setSaveSequenceAs(saveSequenceAs + i);
	}
}

void SaveASequenceScreen::openNameScreen()
{
    if (param == "file")
    {
        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        nameScreen->initialize(sequencer.lock()->getActiveSequence()->getName(), 16, [this](std::string&) {
            openScreen(name);
        }, name);
        openScreen("name");
    }
}

void SaveASequenceScreen::function(int i)
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
        auto fileName = mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()) + ".MID";
        auto disk = mpc.getDisk();

		if (disk->checkExists(fileName))
		{
            auto replaceAction = [this, disk, fileName]{
                auto success = disk->getFile(fileName)->del();

                if (success)
                {
                    disk->flush();
                    disk->initFiles();
                    disk->writeMid(sequencer.lock()->getActiveSequence(), fileName);
                }
            };

            const auto initializeNameScreen = [this]{
                auto nameScreen = mpc.screens->get<NameScreen>("name");
                auto enterAction = [this](std::string&){ openScreen(name); };
                nameScreen->initialize(nameScreen->getNameWithoutSpaces(), 16, enterAction, "save");
            };

            auto fileExistsScreen = mpc.screens->get<FileExistsScreen>("file-exists");
            fileExistsScreen->initialize(replaceAction, initializeNameScreen, [this]{ openScreen("save"); });
            openScreen("file-exists");
			return;
		}
		
		auto seq = sequencer.lock()->getActiveSequence();
		
		disk->writeMid(seq, fileName);
		break;
    }
    }
}

void SaveASequenceScreen::displaySaveAs()
{
	findField("saveas")->setText("MIDI FILE TYPE " + std::to_string(saveSequenceAs));
}

void SaveASequenceScreen::displayFile()
{
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	auto name = nameScreen->getNameWithoutSpaces();

	if (name.length() < 2)
	{
		return;
	}

	findField("file")->setText(name.substr(0, 1));
	findLabel("file1")->setText(name.substr(1));
}

void SaveASequenceScreen::setSaveSequenceAs(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}

	saveSequenceAs = i;
	displaySaveAs();
}
