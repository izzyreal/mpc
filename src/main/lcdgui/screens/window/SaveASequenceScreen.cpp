#include "SaveASequenceScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;

SaveASequenceScreen::SaveASequenceScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "save-a-sequence", layerIndex)
{
}

void SaveASequenceScreen::open()
{
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
	else if (param == "file")
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
		nameScreen->parameterName = "save-a-sequence";
		openScreen("name");
	}
}

void SaveASequenceScreen::function(int i)
{
	init();
	auto nameScreen = mpc.screens->get<NameScreen>("name");

	switch (i)
	{
	case 3:
		openScreen("save");
		break;
	case 4:
		auto fileName = mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()) + ".MID";

		if (mpc.getDisk()->checkExists(fileName))
		{
			openScreen("file-exists");
			return;
		}
		
		auto seq = sequencer->getActiveSequence();
		
		mpc.getDisk()->writeMid(seq, fileName);
		openScreen("save");
		break;
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
