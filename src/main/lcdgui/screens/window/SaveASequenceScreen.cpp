#include "SaveASequenceScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

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

	if (param.compare("saveas") == 0)
	{
		setSaveSequenceAs(saveSequenceAs + i);
	}
	else if (param.compare("file") == 0)
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
		nameScreen->parameterName = "savesequencename";
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
		auto fileName = mpc::Util::getFileName(nameScreen->getName()) + ".MID";
		
		if (mpc.getDisk().lock()->checkExists(fileName))
		{
			openScreen("file-exists");
			return;
		}
		
		auto seq = sequencer.lock()->getActiveSequence().lock();
		
		mpc.getDisk().lock()->writeSequence(seq.get(), fileName);
		openScreen("save");
		break;
	}
}

void SaveASequenceScreen::displaySaveAs()
{
	findField("saveas").lock()->setText("MIDI FILE TYPE " + to_string(saveSequenceAs));
}

void SaveASequenceScreen::displayFile()
{
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	auto name = nameScreen->getName();

	if (name.length() < 2)
	{
		return;
	}

	findField("file").lock()->setText(name.substr(0, 1));
	findLabel("file1").lock()->setText(name.substr(1));
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
