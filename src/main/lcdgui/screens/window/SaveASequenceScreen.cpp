#include "SaveASequenceScreen.hpp"

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveASequenceScreen::SaveASequenceScreen(const int layerIndex)
	: ScreenComponent("save-a-sequence", layerIndex)
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
		nameGui->setParameterName("savesequencename");
		ls.lock()->openScreen("name");
	}
}

void SaveASequenceScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
		auto fileName = mpc::Util::getFileName(nameGui->getName()) + ".MID";
		
		if (mpc.getDisk().lock()->checkExists(fileName))
		{
			ls.lock()->openScreen("filealreadyexists");
			return;
		}
		
		auto seq = sequencer.lock()->getActiveSequence().lock();
		
		mpc.getDisk().lock()->writeSequence(seq.get(), fileName);
		ls.lock()->openScreen("save");
		break;
	}
}

void SaveASequenceScreen::displaySaveAs()
{
	findField("saveas").lock()->setText("MIDI FILE TYPE " + to_string(saveSequenceAs));
}

void SaveASequenceScreen::displayFile()
{
	auto name = nameGui->getName();

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
